#include <netdb.h>
#include <stdio.h>  /* for printf and fprintf */
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
#define PORT 9999
#define SA struct sockaddr

int setup_connection(int sockfd)
{

    struct sockaddr_in servaddr;

    // socket create and varification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
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
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    return sockfd;
}




int client_login(a) {



    return 0;
}



int main()
{
    int sockfd;
    // struct sockaddr_in servaddr;

    // // socket create and varification
    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd == -1) {
    //     printf("socket creation failed...\n");
    //     exit(0);
    // }
    // else
    //     printf("Socket successfully created..\n");
    // bzero(&servaddr, sizeof(servaddr));

    // // assign IP, PORT
    // servaddr.sin_family = AF_INET;
    // servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // servaddr.sin_port = htons(PORT);

    // // connect the client socket to server socket
    // if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
    //     printf("connection with the server failed...\n");
    //     exit(0);
    // }
    // else
    //     printf("connected to the server..\n");

    sockfd = setup_connection(sockfd);




    for (;;)
    {
        // char buf[64] = {0}; // buffer to read in from client
        // read(sockfd, buf, 64);
        // printf("%s\n", buf);

\
        printf(">> ");
        char buffer[1024] = {0};
        // translate the commands into numbers
        char str[1024];
        fgets(str, 1024, stdin);
        send(sockfd, str, strlen(str), 0);



        // // create flags for function calls, and then go
        // char buf[64] = {0}; // buffer to read in from client
        // read(sockfd, buf, 64);
        // printf("%s\n", buf);

    }
    return EXIT_SUCCESS;
}
