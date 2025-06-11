#include "pkt_utils.h"
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

#define WINDOW 3
#define PACKETS 13

/* Sends the 26 bytes of the alphabet over the socket described by sockfd to
 * dest. Also, introduces bit error at the rate of ber. */
void gbn_send_alphabet_to(int sockfd, struct sockaddr_in dest, double ber) {
    packet_t packets[PACKETS];
    //checks if packets have been sent with 0 as not sent 1 as sent
    uint8_t send_packets[PACKETS];

    //create packets to send
    char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    for(int i = 0; i < PACKETS; ++i){
        char data[PKT_DATA_SIZE];
        data[0] = alpha[i * 2];
        data[1] = alpha[i * 2 + 1];
        
        pkt_build(&packets[i], PKT_TYPE_DATA, data, 0);
        printf("Built packet: ");
        pkt_print(&packets[i]);
    }

    printf("\n---------Beginning Go-Back-N Transmission---------\n");

    uint8_t send_base = 0; //base of window
    uint8_t next_pkt = 0; //next in sequence to send

    while(send_base < PACKETS){
        while(next_pkt < send_base + WINDOW && next_pkt < PACKETS){
            //Need to maintain previous packets before we induce error
            packet_t send_pkt = packets[next_pkt];
            introduce_bit_error((char *)send_pkt.bytes, PKT_SIZE, ber);
            printf("Sending packet %d", next_pkt);
            pkt_print(&send_pkt);
            send_pkt = pkt_hton(send_pkt);
            if(sendto(sockfd, send_pkt.bytes, PKT_SIZE, 0, (struct sockaddr *)&dest, sizeof(dest)) < 0){
                perror("Send failed ");
                return;
            }
            printf("Packet %d transmitted", next_pkt);
            send_packets[next_pkt] = 1;
            next_pkt++;
        }

        //Get response
        packet_t response;
        if(recvfrom(sockfd, response.bytes, PKT_SIZE, 0, NULL, NULL)){
            perror("Receive failed ");
            return;
        }

        response = pkt_ntoh(response);

        //If we received an ACK
        if(response.type == PKT_TYPE_ACK){
            printf("Response of ACK received for packet %d", response.sequence_number - 1);
            send_base = response.sequence_number;
            next_pkt = send_base;
        }

        //If we received a NAK
        if(response.type == PKT_TYPE_NAK){
            printf("Response of NAK received for packet %d, retransmitting from %d", response.sequence_number - 1, send_base);
            next_pkt = send_base;
        }

        //If we send all packets
        if(send_base == PACKETS){
            printf("Transmitted all packets!");
            return;
        }
    }
}