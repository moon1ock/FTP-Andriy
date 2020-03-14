#include "server_functions.h"
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#define MAX 80
#define PORT 9999
#define SA struct sockaddr
#define USERCOUNT 5

struct logindb{
    /* used to get and store the user names and passwords */

    char usrn[20];
    char pass[20];
};

int server_setup(int port, int usercount){

    int server_socket, client_socket;
    struct sockaddr_in servaddr;

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Socket creation failed...\n");
        exit(EXIT_FAILURE);
    }
    else
        printf("Socket successfully created..\n");

    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if ((bind(server_socket, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("Socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verify up to 5 connections
    if ((listen(server_socket, USERCOUNT)) < 0)
    {
        printf("Listen failed...\n");
        exit(EXIT_FAILURE);
    }
    printf("server socket %d\n", server_socket);
    return server_socket;
}

int accept_client(int sockfd){

    struct sockaddr_in clients;
    unsigned int addr_size = sizeof(struct sockaddr_in);
    int client_socket = accept(sockfd, (SA *)&clients, &addr_size);
    if (client_socket < 0)
    {
        printf("Server acccept failed...\n");
        exit(0);
    }
    else
        printf("New client accepted...\n");

    printf("client socket %d\n", client_socket);
    return client_socket;
}

int user_login(int client_socket, struct logindb * logins) {

    int inflow, fg = 0, i;
    char buffer[1024] = {0}; // buffer to read in from client
    char parsed[64] = {0}; // for command parsing


    //handle login
    inflow = read(client_socket, buffer, 1024);

    char *key;
    if (buffer[4] == ' ')
        key = strtok(buffer, " ");
    else {printf("Please, authenticate by entering your username\n"); send(client_socket, "Please, authenticate by entering your username\n", strlen("Please, authenticate by entering your username\n"), 0); return 0;}

    if (!strcmp(key, "USER")){
        for (int i = 5; i < 64; i++)
            parsed[i-5] = buffer[i];}
    else {printf("Please, authenticate by entering your username\n"); send(client_socket, "Please, authenticate by entering your username\n", strlen("Please, authenticate by entering your username\n"), 0); return 0;}


    key = strtok(parsed, "\n");


    for ( i = 0; i < USERCOUNT; i++){
        if (!strcmp(logins[i].usrn, key)){
            printf("User identified!\n"); fg = 1;
            send(client_socket, "User identified!\n", strlen("User identified!\n"), 0);
            break;}
    }
    if (!fg){printf("This username not found\nPlease set username in the db.txt file\n"); send(client_socket, "This username not found\nPlease set username in the db.txt file\n", strlen("This username not found\nPlease set username in the db.txt file\n"), 0); return 0;}

    // handle password
    inflow = read(client_socket, buffer, 64);

    key = strtok(buffer, " ");

    if (!strcmp(key, "PASS")){
        //printf("PASS command received!\n");
        for (int j = 5; j<20; j++) {
            parsed[j-5] = buffer[j];
        }
    }
    else {printf("'PASS' command not received!\nAuthentication failed, please try again!\n"); send(client_socket, "'PASS' command not received!\nAuthentication failed, please try again!\n", strlen("'PASS' command not received!\nAuthentication failed, please try again!\n"), 0); return 0;}
    //send(client_socket, "505", strlen("505"), 0);

    key = strtok(parsed, "\n");

    if(!strcmp(logins[i].pass, key)){
            printf("Password accepted!\n");
            char wel_usr[64];
            sprintf(wel_usr,"Authentication complete! Welcome %s\n", logins[i].usrn);
            send(client_socket, wel_usr,strlen(wel_usr), 0);
            return 1;}
    else {printf("Authentication failed, try again!\n"); send(client_socket, "Authentication failed, try again!\n", strlen("Authentication failed, try again!\n"), 0);}

    return 0;
}

void getusers(struct logindb * logins){

    /*read the db.txt file, parse it, and populate the structure of users*/

    FILE* db = fopen("db.txt", "r");
    if(!db) {printf("User Login Data not found!\n");}

    char str[50];
    char *entry;

    int i = 0;
    while (i < USERCOUNT){
        //printf("%s", str);}
        fgets(str, 50, db);

        entry = strtok(str, "\n");
        //printf("%s", entry);
        if (!entry) printf("Password file corrupted\n");
        strcpy(logins[i].usrn, entry);

        fgets(str, 50, db);
        entry = strtok(str, "\n");
        //printf("%s", entry);
        if (!entry) printf("Password file corrupted\n");
        strcpy(logins[i].pass, entry);

        i++;
    }
    fclose(db);
    return;
}


// you can send command / filesize to parse easier



void echo(int client_socket) {


    char buffer[64] = {0}; // buffer to read in from client

    //handle login
    read(client_socket, buffer, 64);
    send(client_socket, buffer, strlen(buffer), 0);

    return;
}
