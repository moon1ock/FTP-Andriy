
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <dirent.h>
#include <sys/time.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <limits.h>
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
    if (server_socket == -1){
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
    if (buffer[4] == ' ' && isalpha(buffer[5]))
        key = strtok(buffer, " ");
    else {printf("user not authenticated!\n"); send(client_socket, "Please, authenticate by entering your username\n", strlen("Please, authenticate by entering your username\n"), 0); return 0;}

    if (!strcmp(key, "USER")){
        for (int i = 5; i < 64; i++)
            parsed[i-5] = buffer[i];}
    else {printf("user trying to authenticate\n"); send(client_socket, "Please, authenticate by entering your username\n", strlen("Please, authenticate by entering your username\n"), 0); return 0;}


    key = strtok(parsed, "\n");


    for ( i = 0; i < USERCOUNT; i++){
        if (!strcmp(logins[i].usrn, key)){
            printf("User identified!\n"); fg = 1;
            send(client_socket, "User identified!\n", strlen("User identified!\n"), 0);
            break;}
    }
    if (!fg){printf("user used a wrong username\n"); send(client_socket, "This username not found\nPlease set username in the db.txt file\n", strlen("This username not found\nPlease set username in the db.txt file\n"), 0); return 0;}

    // handle password
    inflow = read(client_socket, buffer, 64);
    if (buffer[4] == ' ')
        key = strtok(buffer, " ");
     else {printf("user misused the PASS command\n"); send(client_socket, "'PASS' command used incorrectly!\nAuthentication failed, please try again!\n", strlen("'PASS' command used incorrectly!\nAuthentication failed, please try again!\n"), 0); return 0;}
    //key = strtok(buffer, " ");

    if (!strcmp(key, "PASS")){
        //printf("PASS command received!\n");
        for (int j = 5; j<20; j++) {
            parsed[j-5] = buffer[j];
        }
    }
    else {printf("user did not send the PASS command\n"); send(client_socket, "'PASS' command not received!\nAuthentication failed, please try again!\n", strlen("'PASS' command not received!\nAuthentication failed, please try again!\n"), 0); return 0;}
    //send(client_socket, "505", strlen("505"), 0);

    key = strtok(parsed, "\n");

    if(!strcmp(logins[i].pass, key)){
            printf("user authentified successfully!\n");
            char wel_usr[64];
            sprintf(wel_usr,"Authentication complete! Welcome, %s!\n", logins[i].usrn);
            send(client_socket, wel_usr,strlen(wel_usr), 0);
            return 1;}
    else {printf("user failed to authenticate\n"); send(client_socket, "Authentication failed, try again!\n", strlen("Authentication failed, try again!\n"), 0);}

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

void echo(int client_socket, char *buffer) {

    send(client_socket, buffer, strlen(buffer), 0);

    return;
}

void getpwd(int client_socket) {
    /*simple function to return the current directory of the server*/
    char buffer[64] = {0};
   if (getcwd(buffer, sizeof(buffer)) != NULL){
       strcat(buffer, "\n");
       send(client_socket, buffer, strlen(buffer), 0);}
   else{
       strcpy(buffer,"[-] getcwd() error\n" );
       send(client_socket, buffer, strlen(buffer), 0);}

    return;
}


void getls(int client_socket) {
    /*function that lists the current files in the server directory*/
    DIR *d;
    struct dirent *dir;
    char buffer[1024] = {0};
    char tmpbuf[64] = {0};
    d = opendir(".");

    if (d){
        while ((dir = readdir(d)) != NULL)
        {
            sprintf(tmpbuf, "%s\n", dir->d_name);
            strcat(buffer, tmpbuf);
        }
        closedir(d);
    }
    else{sprintf(buffer, "[-] permission or directory error (i doubt it will ever happen though)!!!\n");}

   send(client_socket, buffer, strlen(buffer), 0);

    return;
}


void changedir(int client_socket, char *buffer) {
    /*function that changes the current server directory*/

    char *key;
    char buf[64]; // store the path
    for (int i = 3; i < 64; i++)
            buf[i-3] = buffer[i];


    key = strtok(buf, "\n");

    if(chdir(buf) == 0)
        getpwd(client_socket);
    else
        echo(client_socket, "CD error!\n");

    return;
}




int funcall(char *buf) {

    char buffer[64];// create a copy bc strtok is destructive
    strcpy(buffer, buf);
    char *key;
    if (strstr(buffer, " ") ){
        key = strtok(buffer, " ");
        printf("there is a space in the user inquiry\n");
        if(!strcmp(key, "CD")){
            printf("CD command received!\n"); return 2;}
    }
    else if(buffer[0]!='\n'){
        key = strtok(buffer, "\n");
        if(!strcmp(key, "PWD")){
            printf("PWD command received!\n"); return 1;}
        else if(!strcmp(key, "LS")){
            printf("LS command received!\n"); return 3;}
    }

    return 0;
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

    int query = 0;
    int auth[FD_SETSIZE] = {0}; // creating a boolean array like [0, 0, 0, 1], and treat 0 as unidentified, and 1 as identified users

    for (;;)
    {
        // copy sockets because select is destructive
        check_sockets = sockets;

        if (select(FD_SETSIZE, &check_sockets, NULL, NULL, NULL) < 0) {
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
                    // force the authentification
                    if (!auth[i]){
                        auth[i] = user_login(i, logins);}

                    else{

                        // get the client query, and process it
                        char buffer[64] = {0};
                        read(i, buffer, 64);

                        query = funcall(buffer);

                        switch(query){
                            case 1: //PWD server command
                                getpwd(i);
                                break;
                            case 2: // CD server command
                                changedir(i, buffer);
                                break;
                            case 3: // LS server command
                                getls(i);
                                break;
                            case 4: // QUIT command
                                break;
                            default:
                               echo(i, buffer);
                        }
                    }

                }
            }
        }

    }

    return EXIT_SUCCESS;
}
