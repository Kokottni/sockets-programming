#include "gbn_receiver.h"
#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SEND_BUF_SIZE 1024

struct receiver_args {
    uint16_t port_ns; // network short byte ordering
};

bool parse_args(struct receiver_args *args, int argc, char **argv);

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in serv_addr;
    struct receiver_args args;

    // parse arguments into args struct
    if (!parse_args(&args, argc, argv))
        return 1;

    // set up server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = args.port_ns;

    // create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // seed rand
    srand(time(NULL));

    // bind socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error binding socket");
        exit(1);
    }

    gbn_receive(sockfd);
}

// parse command line arguments into args struct
bool parse_args(struct receiver_args *args, int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port> \n", argv[0]);
        return false;
    }

    args->port_ns = htons(atoi(argv[1]));

    return true;
}