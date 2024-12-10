#ifndef LIST_H
#define LIST_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


// User node structure
struct node {
   char username[30];
   int socket;
   struct node *next;
};


// Room structure
struct room {
   char room_name[30];
   struct node *user_list; // Users in the room
   struct room *next;
};


// User list management
struct node* insertFirstU(struct node *head, int socket, char *username);
struct node* findU(struct node *head, char *username);


// Room management
struct room* createRoom(struct room *head, char *room_name);
struct room* findRoom(struct room *head, char *room_name);
struct room* addUserToRoom(struct room *head, char *room_name, char *username);
void removeUserFromRoom(struct room *head, char *room_name, char *username);


#endif
