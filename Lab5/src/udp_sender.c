#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "gbn_sender.h"

struct sender_args {
    in_addr_t ip;
    uint16_t dest_port_ns; // network short byte ordering
    double ber;
};

bool parse_args(struct sender_args *args, int argc, char **argv);

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in dest_addr;
    struct sender_args args;

    // parse arguments into args struct
    if (!parse_args(&args, argc, argv))
        return 1;

    // set up server address
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = args.ip;
    dest_addr.sin_port = args.dest_port_ns;

    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }
    
    // seed rand
    srand(time(NULL));

    gbn_send_alphabet_to(sockfd, dest_addr, args.ber);

    return 0;
}

// parse command line arguments into args struct
bool parse_args(struct sender_args *args, int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <ip> <dest_port> <ber>\n", argv[0]);
        return false;
    }

    args->ip = inet_addr(argv[1]);
    args->dest_port_ns = htons(atoi(argv[2]));
    args->ber = atof(argv[3]);

    return true;
}