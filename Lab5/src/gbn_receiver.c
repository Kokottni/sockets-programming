#include "crc.h"
#include "pkt_utils.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>

#define NUM_EXPECTED_PACKETS 13

/* Wrapping common, correlated variables in a struct can help
 * tidy our function signatures and make things concise. Personal preference */
typedef struct {
	int sockfd;
	struct sockaddr_in *client_addr;
	socklen_t addr_len;
} gbn_context_t;

/* Small, verbosely-named functions can help the main structure of our code
 * read cleanly. It's must easier to read gbn_receive with aptly-named helpers. */
static bool receive_packet(gbn_context_t *ctx, packet_t *pkt);
static bool is_packet_valid(packet_t *pkt);
static void handle_corrupted(gbn_context_t *ctx, packet_t *pkt, uint8_t expected);
static void handle_in_order(gbn_context_t *ctx, packet_t *pkt, uint8_t *expected);
static void handle_out_of_order(gbn_context_t *ctx, packet_t *pkt, uint8_t expected);
static void send_response(gbn_context_t *ctx, packet_t *response);

void gbn_receive(int sockfd) {
	struct sockaddr_in client_addr;
	socklen_t addr_len = sizeof(client_addr);
	gbn_context_t ctx = {sockfd, &client_addr, addr_len};

	printf("\n---------Beginning subroutine---------\n");

	while (true) {
		uint8_t expected = 0;
		while (expected < NUM_EXPECTED_PACKETS) {
			packet_t received;
			if (!receive_packet(&ctx, &received))
				continue;

			if (is_packet_valid(&received)) {
				if (received.sequence_number == expected) {
					handle_in_order(&ctx, &received, &expected);
				} else {
					handle_out_of_order(&ctx, &received, expected);
				}
			} else {
				handle_corrupted(&ctx, &received, expected);
			}
		}

		printf("\n---------Restarting subroutine---------\n");
	}
}

static bool is_packet_valid(packet_t *packet) {
	uint16_t expected_crc = crc_calculate(packet->bytes, PKT_SIZE - 2);
	return packet->crc_sum == expected_crc;
}

static void send_response(gbn_context_t *ctx, packet_t *response) {
	printf("\t-> Sending: ");
	pkt_print(response);
	if (sendto(ctx->sockfd, response->bytes, sizeof(*response), 0,
			   (struct sockaddr *)ctx->client_addr, ctx->addr_len) < 0) {
		perror("Send failed");
	}
}

static void handle_corrupted(gbn_context_t *ctx, packet_t *pkt, uint8_t expected) {
	pkt_print(pkt);
	uint16_t expected_crc = crc_calculate(pkt->bytes, PKT_SIZE - 2);
	printf("\t-> Expected CRC: %x, Received CRC: %x\n", expected_crc, pkt->crc_sum);

	packet_t nak;
	char pkt_data[] = "\0\0";
	pkt_build(&nak, PKT_TYPE_NAK, pkt_data, expected);
	send_response(ctx, &nak);
}

static void handle_in_order(gbn_context_t *ctx, packet_t *pkt, uint8_t *expected) {
	pkt_print(pkt);

	packet_t ack;
	char pkt_data[PKT_DATA_SIZE] = {'\0', '\0'};
	pkt_build(&ack, PKT_TYPE_ACK, pkt_data, ++(*expected));
	send_response(ctx, &ack);
}

static void handle_out_of_order(gbn_context_t *ctx, packet_t *pkt, uint8_t expected) {
	pkt_print(pkt);
	printf("\t-> Out of order, expected %d\n", expected);

	packet_t ack;
	char pkt_data[PKT_DATA_SIZE] = {'\0', '\0'};
	pkt_build(&ack, PKT_TYPE_ACK, pkt_data, expected);
	send_response(ctx, &ack);
}

static bool receive_packet(gbn_context_t *ctx, packet_t *pkt) {
	int read_size = recvfrom(ctx->sockfd, pkt->bytes, PKT_SIZE, 0,
							 (struct sockaddr *)ctx->client_addr, &ctx->addr_len);
	if (read_size < 0) {
		perror("recv failed");
		return false;
	}

	*pkt = pkt_ntoh(*pkt);
	return true;
}
