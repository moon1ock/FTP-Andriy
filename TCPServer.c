#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h> 
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define USERCOUNT 5



int server_setup(int port, int usercount){
    int server_socket, client_socket;
    struct sockaddr_in servaddr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Socket creation failed...\n");
        exit(EXIT_FAILURE);}
    else
        printf("Socket successfully created..\n");


    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);


    if ((bind(server_socket, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("Socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verify up to 5 connections
    if ((listen(server_socket, USERCOUNT)) < 0) {
        printf("Listen failed...\n");
        exit(EXIT_FAILURE);
    }

    return server_socket;
}



int accept_client(int sockfd){

    struct sockaddr_in clients;
    unsigned int addr_size = sizeof(struct sockaddr_in);
    int client_socket = accept(sockfd, (SA*)&clients, &addr_size);
    if (client_socket < 0) {
        printf("Server acccept failed...\n");
        exit(0);}
    else    printf("New client accepted...\n");

    return client_socket;
}

void handle_connection(int client_socket){


    int inflow;
    char buffer[2048] = {0};


    while(1){
    inflow = read(client_socket, buffer, 2048);
    printf("%s\n",buffer);
    //send(client_socket , "Hello client!" , strlen("Hello client!") , 0 );
    //printf("Hello client msg sent!\n");
    }
}


int main()
{

    int sockfd = server_setup(PORT, USERCOUNT);

    for (;;){

        // accept new connections
        printf("Waiting for connections...\n");
        int client_s = accept_client(sockfd);


        // process incomming requests
        handle_connection(client_s);

    }

    return EXIT_SUCCESS;
}
