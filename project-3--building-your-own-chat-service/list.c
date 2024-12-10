#include "list.h"


// Insert a user into the user list
struct node* insertFirstU(struct node *head, int socket, char *username) {
   if (findU(head, username) == NULL) {
       struct node *link = (struct node*) malloc(sizeof(struct node));
       link->socket = socket;
       strcpy(link->username, username);
       link->next = head;
       head = link;
   }
   return head;
}


// Find a user by username
struct node* findU(struct node *head, char *username) {
   struct node *current = head;
   while (current != NULL) {
       if (strcmp(current->username, username) == 0)
           return current;
       current = current->next;
   }
   return NULL;
}


// Create a room
struct room* createRoom(struct room *head, char *room_name) {
   printf("Attempting to create room: %s\n", room_name); // Debug
   if (findRoom(head, room_name) == NULL) {
       struct room *newRoom = (struct room *)malloc(sizeof(struct room));
       strcpy(newRoom->room_name, room_name);
       newRoom->user_list = NULL;
       newRoom->next = head;
       head = newRoom;
       printf("Room created: %s\n", room_name); // Debug
   } else {
       printf("Room already exists: %s\n", room_name); // Debug
   }
   return head;
}


// Find a room by name
struct room* findRoom(struct room *head, char *room_name) {
   struct room *current = head;
   while (current != NULL) {
       if (strcmp(current->room_name, room_name) == 0)
           return current;
       current = current->next;
   }
   return NULL;
}


// Add a user to a room
struct room* addUserToRoom(struct room *head, char *room_name, char *username) {
   struct room *room = findRoom(head, room_name);
   if (room) {
       room->user_list = insertFirstU(room->user_list, -1, username); // -1 for no socket lookup
   }
   return head;
}


// Remove a user from a room
void removeUserFromRoom(struct room *head, char *room_name, char *username) {
   struct room *room = findRoom(head, room_name);
   if (room) {
       struct node *prev = NULL, *current = room->user_list;
       while (current && strcmp(current->username, username) != 0) {
           prev = current;
           current = current->next;
       }
       if (current) {
           if (prev)
               prev->next = current->next;
           else
               room->user_list = current->next;
           free(current);
       }
   }
}
