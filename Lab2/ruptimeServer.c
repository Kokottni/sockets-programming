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

#define PORT 3490
#define BACKLOG 10
#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    int serverfd, incomingfd;
    struct sockaddr_in server_addy;
    struct sockaddr_in cliaddr;
    socklen_t addrlen = sizeof(server_addy);
    int clibytes = 0;
    char buff[BUFFER_SIZE];
    char *uptime_cmd = "uptime";

    serverfd = socket(PF_INET, SOCK_STREAM, 0);
    if((serverfd <= 0))
    {
        perror("socket failed.");
        exit(EXIT_FAILURE);
    }

    server_addy.sin_family = PF_INET;
    server_addy.sin_port = htons(PORT);
    server_addy.sin_addr.s_addr = INADDR_ANY;


    if(bind(serverfd, (struct sockaddr*)&server_addy, addrlen) == -1)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    
    if(listen(serverfd, BACKLOG) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while(1)
    {
        incomingfd = accept(serverfd, (struct sockaddr*)&cliaddr, (socklen_t*)&addrlen);
        if(incomingfd <= 0)
        {
            perror("client socket");
            exit(EXIT_FAILURE);
        }
        FILE *fp = popen(uptime_cmd, "r");
        if(fp == NULL)
        {
            perror("popen failed");
            close(incomingfd);
            continue;
        }

        fgets(buff, BUFFER_SIZE, fp);
        pclose(fp);

        if(write(incomingfd, buff, BUFFER_SIZE) == -1)
        {
            perror("write failed");
            close(incomingfd);
            continue;
        }

        close(incomingfd);
    }
}