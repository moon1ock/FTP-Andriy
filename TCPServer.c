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
#define PORT 8888
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

int user_login(int client_socket, struct logindb * logins)
{

    int inflow, fg = 0, i;
    char buffer[1024] = {0}; // buffer to read in from client
    char parsed[20] = {0}; // for command parsing


    //handle login
    inflow = read(client_socket, buffer, 1024);

    char *key;

    key = strtok(buffer, " ");

    if (!strcmp(key, "USER")){
        for (int i = 5; i<20; i++)
            parsed[i-5] = buffer[i];
    }
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





int main() {

    struct logindb logins[USERCOUNT]; //reading <up to USERCOUNT> users into the login-database
    getusers(logins);

    int sockfd = server_setup(PORT, USERCOUNT);

    fd_set sockets, check_sockets;

    // TODO: deal with the largest FDSETSIZE and check only the filled connections,
    // as opposed to all of them (1024) for that matter

    FD_ZERO(&sockets);
    FD_ZERO(&check_sockets);
    FD_SET(sockfd, &sockets);

    int k = 0; // creating a boolean array like [0, 0, 0, 1], and treat 0 as unidentified, and 1 as identified users
    int auth[FD_SETSIZE] = {0};

    for (;;)
    {
        // copy sockets because select is destructive
        check_sockets = sockets;

        if (select(FD_SETSIZE, &check_sockets, NULL, NULL, NULL) < 0)
        {
            printf("select error\n");
            return EXIT_FAILURE;
        }

        for (int i = 0; i < FD_SETSIZE; i++)
        {
            if (FD_ISSET(i, &check_sockets))
            {
                if (i == sockfd)
                {
                    //a new upcomming connection
                    printf("adding a new connection\n");
                    int client_s = accept_client(sockfd);
                    FD_SET(client_s, &sockets);
                }
                else
                {

                    if (!auth[i]){
                        auth[i] = user_login(i, logins);}

                    else echo(i);

                }
            }
        }

    }

    return EXIT_SUCCESS;
}
