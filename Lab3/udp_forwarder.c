/* udp_forwarder.c
 *
 * Author:
 * Description: Forwards UDP packets from a source to a destination.
 *
 * Usage:
 *   Compile the program:
 *     make
 *   Run it:
 *     ./udp-forwarder <SERVER_IP> <SERVER_PORT> <DESTINATION_IP> <DESTINATION_PORT> <LOSS_RATE>
 *   Loss rate is the number of packets out of 1000 that are dropped.
 *
 * Tips:
 *   Check the man pages for any functions you're unsure of. Parameters,
 *   return values, and sometimes even examples are listed!
 *
 *   Check for errors, and abstract logically separate blocks into separate
 *   functions!
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 2048

/* main
 * The main entry point of your program */
int main(int argc, char **argv)
{
    // Good luck! Let you TA know if you have any questions.
    if(argc != 6)
    {
        fprintf(stderr, "Usage: %s <SERVER_IP> <SERVER_PORT> <DESTINATION_IP> <DESTINATION_PORT> <LOSS_RATE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    //instantiate server and destination socket file descriptors
    int serverfd, destfd, loss_rate;
    //instantiate server and destination addresses, need client to take in video data
    struct sockaddr_in server_addr, dest_addr, cli_addr;
    //determine address length of server and dest
    socklen_t servaddrlen = sizeof(server_addr);
    socklen_t dstaddrlen = sizeof(dest_addr);
    //initialize buffer
    char buffer[BUFFER_SIZE];

    //setup server socket and error check
    serverfd = socket(PF_INET, SOCK_DGRAM, 0);
    if(serverfd <= 0)
    {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }

    //setup destination socket and error check
    destfd = socket(PF_INET, SOCK_DGRAM, 0);
    if(destfd <= 0)
    {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }
    
    //setup server and destination addresses while checking ports
    if(atoi(argv[2]) < 1024 || atoi(argv[2]) > 65535)
    {
        fprintf(stderr, "Port must be within 1024 and 65535\n");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(atoi(argv[4]) < 1024 || atoi(argv[4]) > 65535)
    {
        fprintf(stderr, "Port must be within 1024 and 65535\n");
        exit(EXIT_FAILURE);
    }
    dest_addr.sin_family = PF_INET;
    dest_addr.sin_port = htons(atoi(argv[4]));
    dest_addr.sin_addr.s_addr = inet_addr(argv[3]);

    //ensure loss rate is within bounds
    loss_rate = atoi(argv[5]);
    if(loss_rate < 0 || loss_rate > 1000)
    {
        fprintf(stderr, "Loss rate must be between 0 and 1000\n");
        exit(EXIT_FAILURE);
    }

    //bind the server socket and error check
    if(bind(serverfd, (const struct sockaddr*)&server_addr, servaddrlen))
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    //main loop to receive and forward packets
    while(1)
    {
        //receive packets from client (us)
        size_t bytes = recvfrom(serverfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&cli_addr, sizeof(cli_addr));
        if(bytes < 0)
        {
            perror("recvfrom failed");
            exit(EXIT_FAILURE);
        }

        //simulate packet loss
        if(rand() % 1000 >= loss_rate)
        {
            //send packets to destination address and error check
            size_t sent = sendto(destfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&dest_addr, dstaddrlen);
            if(sent < 0)
            {
                perror("sendto failed");
                exit(EXIT_FAILURE);
            }
        }
    }
    //close sockets
    close(serverfd);
    close(destfd);
    return 0;
}