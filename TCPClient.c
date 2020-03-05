#include <netdb.h>
#include <stdio.h> /* for printf and fprintf */
#include <string.h> /* for memset */
#include <stdio.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h> /* for socket, connect, send, and recv */
#define MAX 80
#define PORT 8080
#define SA struct sockaddr


int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");


    for(;;){
    char buffer[1024] = {0};
    char str[1024];
    fgets(str, 1024, stdin);
    send(sockfd , str , strlen(str) , 0 );
    //printf("fuck message sent\n");
    //int valread = read( sockfd, buffer, 1024);
    //printf("%s\n",buffer );
    }
    return EXIT_SUCCESS;
}
