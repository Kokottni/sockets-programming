#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 3490

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        fprintf(stderr, "Need to include the ip to connect to: %s <IP>", argv[0]);
        exit(EXIT_FAILURE);
    }
    int clientfd, numbytes;
    struct sockaddr_in server_addr;
    int addrlen = sizeof(server_addr);
    char buff[BUFFER_SIZE];
    char *uptime_cmd = "uptime";

    clientfd = socket(PF_INET, SOCK_STREAM, 0);
    if((clientfd == 0))
    {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if(connect(clientfd, (struct sockaddr*)&server_addr, addrlen))
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

    if(read(clientfd, buff, BUFFER_SIZE) == -1)
    {
        perror("read failed");
        exit(EXIT_FAILURE);
    }
    printf("%s: %s", argv[1], buff);

    close(clientfd);
}

