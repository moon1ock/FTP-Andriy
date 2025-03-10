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
#define DTPORT 9998 // port for a separate TCP connection for data transfer
#define SA struct sockaddr
int sig_handl_sock;


int setup_connection(int sockfd, int port)
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


    int enable = 1; // clear the previous connections
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed\n");
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(port);

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
    {   printf("\033[32;1m");
        printf("%s", buffer);
        printf("\033[0m");
        return 1;
    }
    else
    {
        printf("%s", buffer);
        return 0;
    }
}

int local_funcall(char *buf){
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
    char buffer[1024] = {0};
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

void getls() {
    /*function that lists the current files in the server directory*/
    DIR *d;
    struct dirent *dir;
    char buffer[1024] = {0};
    char tmpbuf[1024] = {0};
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
        printf("\033[31;1mCD error!\033[0m\n");

    return;
}

int transfer_funcall(int sockfd, char *buf){

    char buffer[1024] = {0};

    strcpy(buffer, buf);
    char *key;
    if (strstr(buffer, " ")) {
        key = strtok(buffer, " ");
        if (!strcmp(key, "PUT")) {
            return 1;}
        else if (!strcmp(key, "GET")) {
            return 2;}
    }

    return 0;
}


int check(int sockfd){

    char ch[8] = {0};
    int a;
    fflush(stdin);
    a = read(sockfd, ch, 8);
    if(a < 1) {printf("server error, try again"); return 0;}

    else {
        char *k;
        k = strtok(ch, "\n");
        if(!strcmp(k, "DATA"))
            return 1;
        else return 0;
    }

}


void evoke_put(int sockfd, char *buf){

    char buffer[1024] = {0}; // create a copy bc strtok is destructive
    char name[1024] = {0};
    for(int i = 4; i<sizeof(buffer); i++){
        buffer[i-4] = buf[i];
    }
    char *key = strtok(buffer, "\n"); // filename
    strcpy(name, key);
    if (key == NULL || strstr(key, " ") || strstr(key, "\t")){
        printf("Incorrect filename!\n");
        return;}

    FILE *fp;
    long int size=0;

    fp=fopen(key,"r");

    if(fp == NULL){
        printf("Cannot open the file to be sent out!!!\n");
        return;
    }


    // create a char array 1024 in size (we can use buffer since we don't need it anymore
    // iterate over the file, until EOF, if 1024 then send and restart the counter
    // have tp figure out the connection stuff


    // creating a new TCP connection with a server!

    // ********************************* //
    send(sockfd, buf, strlen(buf), 0); // identifying a put command for the server


    memset(buffer,0,sizeof(buffer));
    read(sockfd, buffer, 1024);

    printf("%s", buffer);// PUT CMD init

    char ch[1024] = {0};
    printf("Setting up a new TCP connection...\n");


    int k = check(sockfd);
    if (!k) {fclose(fp); return;}

    int new_sockfd;
    new_sockfd = setup_connection(new_sockfd, DTPORT);

    send(new_sockfd, "new TCP connection setup!", strlen("new TCP connection setup!"), 0);
    memset(ch,0,sizeof(ch));
    read(new_sockfd, ch, 1024); // confirmation of TCP setup
    printf("%s\n", ch);

    // let's send the filename
    memset(buffer,0,sizeof(buffer));
    send(new_sockfd, name, strlen(name), 0);
    // filename sent

    // check if filename exists
    memset(ch,0,sizeof(ch));
    read(new_sockfd, ch, 1024);
    printf("%s\n", ch);
    if(strcmp(ch, "Uploading the file...")){
        close(new_sockfd);
        fclose(fp);
        return;
    }


    // transfer the file
    memset(buffer,0,sizeof(buffer));
    int cc;
    int i = 0;
    while( ( cc = fgetc(fp) ) != EOF ){
        buffer[i++] = cc;
        if(i == 1023){ // send the 1024 processed bits to the server, reset the cnt and clear the buffer
            buffer[i] = '\0';
            send(new_sockfd, buffer, strlen(buffer), 0);
            memset(ch,0,sizeof(ch));
            read(new_sockfd, ch, 1024);
            //printf("ch: %s\n", ch);
            i = 0;
            memset(buffer,0,sizeof(buffer));
        }
    }
    buffer[i++] = '\0'; // null character to terminate string
    send(new_sockfd, buffer, strlen(buffer), 0); // send the last batch
    printf("\033[32;1m");
    printf("File sent successfully!\033[0m\n");


    memset(ch,0,sizeof(ch));
    read(new_sockfd, ch, 1024);
    send(new_sockfd, "EOF\n", strlen("EOF\n"), 0); // indicate the end of file here

    // ********************************** //

    close(new_sockfd);
    fclose(fp);


}



void evoke_get(int sockfd, char *buf){

    // check if a file with such a name already exists in the directory first
    char buffer[1024] = {0}; // create a copy bc strtok is destructive
    char name[1024] = {0};
    for(int i = 4; i<sizeof(buffer); i++){
        buffer[i-4] = buf[i];
    }
    char *key = strtok(buffer, "\n"); // filename
    strcpy(name, key);
    if (key == NULL || strstr(key, " ") || strstr(key, "\t")){
        printf("Incorrect filename!\n");
        return;}

    FILE *fp;

    fp=fopen(name,"r");

    if(fp != NULL){
        printf("File with such name already exists in this directory! Cannot receive!\n");
        fclose(fp);
        return;
    }
    fclose(fp);


    // send the GET command to the serever to start the processing!!

    send(sockfd, buf, strlen(buf), 0);


    // clear out the buffer for receiving the file

    memset(buffer,0,sizeof(buffer));
    read(sockfd, buffer, 1024);
    if (!strcmp(buffer, "DATA\n")){
        printf("Setting up the connection...\n");}

    else if(!strcmp(buffer, "404 File does not exist!\n")){
        printf("\033[1;31m404 File does not exist!\033[0m\n\n");
        return;
    }



    char ch[1024] = {0};
    printf("Setting up a new TCP connection...\n");


    int new_sockfd;
    new_sockfd = setup_connection(new_sockfd, DTPORT);

    send(new_sockfd, "new TCP connection setup!", strlen("new TCP connection setup!"), 0);
    memset(ch,0,sizeof(ch));
    read(new_sockfd, ch, 1024); // confirmation of TCP setup
    printf("%s\n", ch);

    /* Receive the file */
    fp=fopen(name,"w");


    memset(ch,0,sizeof(ch));
    read(new_sockfd, ch, 1024); // read the first batch
    int cnt =1;
    while (strcmp(ch, "EOF\n")){
        fprintf(fp, "%s", ch); // append the first batch to the file
        send(new_sockfd, "1", strlen("1"), 0); // send confirmation
        memset(ch,0,sizeof(ch)); // clear buffer
        read(new_sockfd, ch, 1024); // read again
    }
    printf("\033[32;1m");
    printf("File received successfully!\033[0m\n");

    // transfer the file
    // memset(buffer,0,sizeof(buffer));
    // int cc;
    // int i = 0;
    // while( ( cc = fgetc(fp) ) != EOF ){
    //     buffer[i++] = cc;
    //     if(i == 1023){ // send the 1024 processed bits to the server, reset the cnt and clear the buffer
    //         buffer[i] = '\0';
    //         send(new_sockfd, buffer, strlen(buffer), 0);
    //         memset(ch,0,sizeof(ch));
    //         read(new_sockfd, ch, 1024);
    //         //printf("ch: %s\n", ch);
    //         i = 0;
    //         memset(buffer,0,sizeof(buffer));
    //     }
    // }
    // buffer[i++] = '\0'; // null character to terminate string
    // send(new_sockfd, buffer, strlen(buffer), 0); // send the last batch
    // printf("\033[32;1m");
    // printf("File sent successfully!\033[0m\n");


    // memset(ch,0,sizeof(ch));
    // read(new_sockfd, ch, 1024);
    // send(new_sockfd, "EOF\n", strlen("EOF\n"), 0); // indicate the end of file here

    // ********************************** //



    close(new_sockfd);
    fclose(fp);


}






void query_handler(int sockfd){
    printf(">> ");
    char str[1024] = {0};
    int query;
    fgets(str, 1024, stdin);


    if(str[0] == '!'){
        query = local_funcall(str);
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
            default:
                send(sockfd, str, strlen(str), 0);
        }
    }
    else {
        query = transfer_funcall(sockfd, str);
        switch (query){
            case 1: //PUT client command
                evoke_put(sockfd, str);
                return;
            case 2: // GET client command
                evoke_get(sockfd, str);
                return;
            default:
                send(sockfd, str, strlen(str), 0);
        }
    }

    if (!strcmp(str, "QUIT\n")){
        exit(0);}
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

    sockfd = setup_connection(sockfd, PORT);
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

