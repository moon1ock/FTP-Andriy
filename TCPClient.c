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
#define MAX 80
#define PORT 9999
#define SA struct sockaddr
int sig_handl_sock;

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

int client_login(int sockfd)
{

    printf(">> ");
    char buffer[1024] = {0};
    // translate the commands into numbers
    char str[1024];
    fgets(str, 1024, stdin);

    send(sockfd, str, strlen(str), 0);

    if (!strcmp(str, "QUIT\n"))
    {
        exit(0);
    }

    read(sockfd, buffer, 1024);

    if (buffer[0] == '5')
    {
        printf("%s", buffer);
        return 1;
    }
    else
    {
        printf("%s", buffer);
        return 0;
    }
}

int funcall(char *buf){
    char buffer[1024] = {0}; // create a copy bc strtok is destructive

    for(int i = 1; i<sizeof(buffer); i++){
        buffer[i-1] = buf[i];
    }
    char *key;

    if (strstr(buffer, " ")) {
        key = strtok(buffer, " ");
        if (!strcmp(key, "CD")) {
            return 2;}
    }
    else if (buffer[0] != '\n') {
        key = strtok(buffer, "\n");
        if (!strcmp(key, "PWD")){
            return 1;}

        else if (!strcmp(key, "LS")){
            return 3;}
    }

    return 0;
}

void getpwd(){
    /*simple function to return the current directory of the server*/
    char buffer[64] = {0};
    if (getcwd(buffer, sizeof(buffer)) != NULL)
    {
        strcat(buffer, "\n");
        printf("%s", buffer);
    }
    else
    {
        strcpy(buffer, "[-] getcwd() error\n");
        printf("%s", buffer);
    }

    return;
}

void getls()
{
    /*function that lists the current files in the server directory*/
    DIR *d;
    struct dirent *dir;
    char buffer[1024] = {0};
    char tmpbuf[64] = {0};
    d = opendir(".");

    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            sprintf(tmpbuf, "%s\n", dir->d_name);
            strcat(buffer, tmpbuf);
        }
        closedir(d);
    }
    else
    {
        sprintf(buffer, "[-] permission or directory error\n");
    }

    printf("%s", buffer);

    return;
}

void changedir(char *buffer){
    /*function that changes the current server directory*/

    char *key;
    char buf[1024] = {0}; // store the path
    for (int i = 4; i < 1024; i++)
        buf[i - 4] = buffer[i];

    key = strtok(buf, "\n");

    if (chdir(buf) == 0)
        getpwd();
    else
        printf("CD error!\n");

    return;
}


void query_handler(int sockfd){
    printf(">> ");
    char str[1024] = {0};
    int query;
    fgets(str, 1024, stdin);


    if(str[0] == '!'){
        query = funcall(str);
        switch (query){
            case 1: //PWD client command
                getpwd();
                return;
            case 2: // CD client command
                changedir(str);
                return;
            case 3: // LS client command
                getls();
                return;
            case 5: // GET command
                return;
            case 6: // PUT command
                return;
            default:
                send(sockfd, str, strlen(str), 0);
        }
    }
    send(sockfd, str, strlen(str), 0);

    if (!strcmp(str, "QUIT\n"))
        exit(0);
    memset(str,0,sizeof(str));
    read(sockfd, str, 1024);
    printf("%s", str);
    return;
}



void sigintHandler(int sig_num){
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    signal(SIGINT, sigintHandler);
    // printf("\n Cannot be terminated using Ctrl+C \n");
    // fflush(stdout);
    send(sig_handl_sock, "QUIT", strlen("QUIT"), 0);
    exit(0);
}

int main(){
    int sockfd, authorized = 0;

    sockfd = setup_connection(sockfd);
    sig_handl_sock = sockfd;
    signal(SIGINT, sigintHandler); // catch CTRL+C

    while (!authorized)
        authorized = client_login(sockfd); // loop to minimize the checks for user auth

    for (;;)
    {
        query_handler(sockfd);
    }
    return EXIT_SUCCESS;
}
