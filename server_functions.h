#ifndef SERVER_FUNCTIONS
#define SERVER_FUNCTIONS
/* ^^ these are the include guards */

/* Prototypes for the functions */
/* Sums two ints */
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

int server_setup(int , int );

int accept_client(int );

int user_login(int , struct logindb * );

void getusers(struct logindb * );


void echo(int );


#endif