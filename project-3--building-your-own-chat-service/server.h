#ifndef SERVER_H
#define SERVER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "list.h"


// Definitions
#define PORT 8888
#define BACKLOG 10
#define MAXBUFF 2096
#define DEFAULT_ROOM "Lobby"
#define TRUE 1
#define FALSE 0


// Shared variables
extern int numReaders;
extern pthread_mutex_t mutex;
extern pthread_mutex_t rw_lock;
extern struct node *head;
extern struct room *room_head;


// Function prototypes
int get_server_socket();
int start_server(int serv_socket, int backlog);
int accept_client(int serv_sock);
void sigintHandler(int sig_num);
void *client_receive(void *ptr);


#endif
