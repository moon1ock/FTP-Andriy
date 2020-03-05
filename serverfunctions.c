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

int accept_client(int sockfd)
{

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

void handle_connection(int client_socket, struct logindb * logins)
{

    int inflow;
    char buffer[64] = {0}; // buffer to read in from client
    char parsed[20] = {0}; // for command parsing

    inflow = read(client_socket, buffer, 64);

    char *key;

    key = strtok(buffer, " ");

    if (!strcmp(key, "USER")){
        printf("USER command received!\n");
        for (int i = 5; i<20; i++) {
            parsed[i-5] = buffer[i];
        }
    }
    else {printf("Command not found!\n"); return;}


    key = strtok(parsed, "\n");

    for (int i = 0; i < USERCOUNT; i++){
        if (!strcmp(logins[i].usrn, key))
            printf("User identified!\n");
    }




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
    // for (int j = 0; j<USERCOUNT/2; j++){
    //     printf("%s   %s", logins[j].pass, logins[j].usrn);
    // }
    return;
}

