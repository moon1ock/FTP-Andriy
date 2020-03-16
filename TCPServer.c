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
#define DTPORT 9998
#define SA struct sockaddr
#define USERCOUNT 5

struct logindb
{
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

    int enable = 1; // clear the previous connections
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        printf("setsockopt(SO_REUSEADDR) failed\n");


    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if ((bind(server_socket, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("Socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");

    // Now server is ready to listen and verify up to 5 connections
    if ((listen(server_socket, usercount)) < 0)
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

int user_login(int client_socket, struct logindb *logins, fd_set *sockets)
{

    int fg = 0, i;
    char buffer[1024] = {0}; // buffer to read in from client
    char parsed[1024] = {0};   // for command parsing

    //handle login
    read(client_socket, buffer, 1024);


    if(!strcmp(buffer, "QUIT") || !strcmp(buffer, "QUIT\n")){ // drop the client on quit
        printf("dropping the client %d\n", client_socket);
        FD_CLR(client_socket, sockets);
        return 0;}


    char *key;
    if (buffer[4] == ' ' && isalpha(buffer[5]))
        key = strtok(buffer, " ");
    else
    {
        printf("user not authenticated!\n");
        send(client_socket, "\033[1;31mPlease, authenticate by entering your username\033[0m\n", strlen("\033[1;31mPlease, authenticate by entering your username\033[0m\n"), 0);
        return 0;
    }

    if (!strcmp(key, "USER"))
    {
        for (int i = 5; i < 1024; i++)
            parsed[i - 5] = buffer[i];
    }
    else
    {
        printf("user trying to authenticate\n");
        send(client_socket, "\033[1;31mPlease, authenticate by entering your username\033[0m\n", strlen("\033[1;31mPlease, authenticate by entering your username\033[0m\n"), 0);
        return 0;
    }

    key = strtok(parsed, "\n");

    for (i = 0; i < USERCOUNT; i++)
    {
        if (!strcmp(logins[i].usrn, key))
        {
            printf("User identified!\n");
            fg = 1;
            send(client_socket, "\033[1mUser identified!\033[0m\n", strlen("\033[1mUser identified!\033[0m\n"), 0);
            break;
        }
    }
    if (!fg)
    {
        printf("user used a wrong username\n");
        send(client_socket, "This username not found\nPlease set username in the db.txt file\n", strlen("This username not found\nPlease set username in the db.txt file\n"), 0);
        return 0;
    }
    memset(buffer,0,sizeof(buffer));
    // handle password
    read(client_socket, buffer, 1024);

    if(!strcmp(buffer, "QUIT") || !strcmp(buffer, "QUIT\n")){ // drop the client on quit
        printf("dropping the client %d\n", client_socket);
        FD_CLR(client_socket, sockets);
        return 0;}



    if (buffer[4] == ' ' && isalpha(buffer[5]))
        key = strtok(buffer, " ");
    else
    {
        printf("user misused the PASS command\n");
        send(client_socket, "'PASS' command used incorrectly!\nAuthentication failed, please try again!\n", strlen("'PASS' command used incorrectly!\nAuthentication failed, please try again!\n"), 0);
        return 0;
    }
    //key = strtok(buffer, " ");

    if (!strcmp(key, "PASS"))
    {
        //printf("PASS command received!\n");
        for (int j = 5; j < 20; j++)
        {
            parsed[j - 5] = buffer[j];
        }
    }
    else
    {
        printf("user did not send the PASS command\n");
        send(client_socket, "'PASS' command not received!\nAuthentication failed, please try again!\n", strlen("'PASS' command not received!\nAuthentication failed, please try again!\n"), 0);
        return 0;
    }
    //send(client_socket, "505", strlen("505"), 0);

    key = strtok(parsed, "\n");

    if (!strcmp(logins[i].pass, key))
    {
        printf("user authentified successfully!\n");
        char wel_usr[1024];
        sprintf(wel_usr, "505 Authentication complete! Welcome, %s!\n", logins[i].usrn);
        send(client_socket, wel_usr, strlen(wel_usr), 0);
        return 1;
    }
    else
    {
        printf("user failed to authenticate\n");
        send(client_socket, "Authentication failed, try again!\n", strlen("Authentication failed, try again!\n"), 0);
    }

    return 0;
}

void getusers(struct logindb *logins)
{

    /*read the db.txt file, parse it, and populate the structure of users*/

    FILE *db = fopen("db.txt", "r");
    if (!db)
    {
        printf("\033[31;1mUser Login Data not found!!\033[0m\n");
    }

    char str[50];
    char *entry;

    int i = 0;
    while (i < USERCOUNT)
    {
        //printf("%s", str);}
        fgets(str, 50, db);

        entry = strtok(str, "\n");
        //printf("%s", entry);
        if (!entry)
            printf("\033[31;1mPassword file corrupted\033[0m\n");
        strcpy(logins[i].usrn, entry);

        fgets(str, 50, db);
        entry = strtok(str, "\n");
        //printf("%s", entry);
        if (!entry)
            printf("\033[31;1mPassword file corrupted\033[0m\n");
        strcpy(logins[i].pass, entry);

        i++;
    }
    fclose(db);
    return;
}

void echo(int client_socket, char *buffer)
{

    send(client_socket, buffer, strlen(buffer), 0);

    return;
}

void getpwd(int client_socket)
{
    /*simple function to return the current directory of the server*/
    char buffer[1024] = {0};
    if (getcwd(buffer, sizeof(buffer)) != NULL)
    {
        strcat(buffer, "\n");
        send(client_socket, buffer, strlen(buffer), 0);
    }
    else
    {
        strcpy(buffer, "\033[31;1mgetcwd() error\033[0m\n");
        send(client_socket, buffer, strlen(buffer), 0);
    }

    return;
}

void getls(int client_socket)
{
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
        sprintf(buffer, "\033[31;1mpermission or directory error!\033[0m\n");
    }

    send(client_socket, buffer, strlen(buffer), 0);

    return;
}

void changedir(int client_socket, char *buffer)
{
    /*function that changes the current server directory*/

    char *key;
    char buf[1024]; // store the path
    for (int i = 3; i < 1024; i++)
        buf[i - 3] = buffer[i];

    key = strtok(buf, "\n");

    if (chdir(buf) == 0)
        getpwd(client_socket);
    else
        echo(client_socket, "\033[31;1mCD error!\033[0m\n");

    return;
}

int funcall(char *buf)
{
    char buffer[1024]; // create a copy bc strtok is destructive
    strcpy(buffer, buf);
    char *key;
    if (strstr(buffer, " "))
    {
        key = strtok(buffer, " ");
        printf("there is a space in the user inquiry\n");
        if (!strcmp(key, "CD"))
        {
            printf("CD command received!\n");
            return 2;
        }
        else if (!strcmp(key, "GET"))
        {
            printf("GET command received!\n");
            return 5;
        }
        else if (!strcmp(key, "PUT"))
        {
            printf("PUT command received!\n");
            return 6;
        }
    }
    else if (buffer[0] != '\n')
    {
        key = strtok(buffer, "\n");
        if (!strcmp(key, "PWD"))
        {
            printf("PWD command received!\n");
            return 1;
        }
        else if (!strcmp(key, "LS"))
        {
            printf("LS command received!\n");
            return 3;
        }
        else if (!strcmp(key, "QUIT"))
        {
            printf("QUIT command received!\n");
            return 4;
        }
    }

    return 0;
}



void doput(int client, char *buffer){

    char *key;
    char buf[1024]; // store the path
    for (int i = 4; i < 1024; i++)
        buf[i - 4] = buffer[i];

    key = strtok(buf, "\n"); // name of the file to put

    //printf("filename: %s\n", key);


    int new_sockfd = server_setup(DTPORT, 5);


    char str[8] = {0};
    strcpy(str, "DATA\n");
    send(client, str, strlen(str), 0);

    printf("Sent data to client!\n");
    // tell client that socket created


    int data = accept_client(new_sockfd);

    char ch[1024] = {0};

    read(data, ch, 1024); // new tcp connection setup

    printf("%s\n", ch);
    send(data, "TCP connection for file transfer established!", strlen("TCP connection for file transfer established!"), 0);

    // let's get the filename
    memset(ch,0,sizeof(ch));
    read(data, ch, 1024);
    char name[1024] = {0};
    strcpy(name, ch);
    //printf("filename: %s\n", ch);
    // filename received here

    FILE *fp;
    long int size=0;

    fp=fopen(name,"r");

    if(fp != NULL){
        printf("File with such name already exists in this directory! Cannot receive!\n");
        send(data, "File with such name already exists in this directory! Cannot receive!", strlen("File with such name already exists in this directory! Cannot receive!"), 0);
        fclose(fp);
        close(new_sockfd);
        close(data);
        return;
    }
    else {
        printf("Downloading the file...\n");
        send(data, "Uploading the file...", strlen("Uploading the file..."), 0);
    }
    fclose(fp);


    // **********RECEIVE THE FILE**********//
    fp=fopen(name,"a");

    if (fp == NULL){
        printf("ERROR WRITING TO A FILE!\n");
        exit(1);
    }


    memset(ch,0,sizeof(ch));
    read(data, ch, 1024); // read the first batch
    int cnt =1;
    while (strcmp(ch, "EOF\n")){
        fprintf(fp, "%s", ch); // append the first batch to the file
        send(data, "1", strlen("1"), 0); // send confirmation
        memset(ch,0,sizeof(ch)); // clear buffer
        read(data, ch, 1024); // read again
    }

    fclose(fp);
    close(data);
    close(new_sockfd);


}



int main()
{
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
                    // force the authentification
                    if (!auth[i])
                    {
                        auth[i] = user_login(i, logins, &sockets);
                    }

                    else
                    {

                        // get the client query, and process it
                        char buffer[1024] = {0};
                        read(i, buffer, 1024);

                        query = funcall(buffer);

                        switch (query)
                        {
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
                            printf("dropping the client %d\n", i);
                            FD_CLR(i, &sockets);
                            break;
                        case 5: // GET command
                            echo(i, "\033[1;GET command!\033[0m\n");
                            break;
                        case 6: // PUT command
                            echo(i, "\033[1;31mPUT command initialized!\033[0m\n");
                            doput(i, buffer);
                            break;
                        default:
                            echo(i, "\033[1;31mInvalid FTP command!\033[0m\n");
                        }
                    }
                }
            }
        }
    }

    return EXIT_SUCCESS;
}
