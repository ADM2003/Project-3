#include "server.h"


void *client_receive(void *ptr) {
   int client = *(int *)ptr; // Client socket
   char buffer[MAXBUFF], cmd[MAXBUFF];
   char username[30];
   char *arguments[80];
   int i;


   // Assign a guest username based on the socket descriptor
   sprintf(username, "guest%d", client);


   pthread_mutex_lock(&mutex);
   head = insertFirstU(head, client, username); // Add the user to the user list
   room_head = addUserToRoom(room_head, DEFAULT_ROOM, username); // Add user to Lobby
   pthread_mutex_unlock(&mutex);


   // Send a welcome message
   sprintf(buffer, "Welcome, %s! You are in the %s room.\nchat>", username, DEFAULT_ROOM);
   send(client, buffer, strlen(buffer), 0);


   while (1) {
       memset(buffer, 0, MAXBUFF); // Clear the buffer


       // Receive a message from the client
       int received = recv(client, buffer, MAXBUFF, 0);
       if (received <= 0) {
           // Client disconnected or error
           printf("User %s disconnected.\n", username);
           pthread_mutex_lock(&mutex);
           removeUserFromRoom(room_head, DEFAULT_ROOM, username);
           pthread_mutex_unlock(&mutex);
           close(client);
           pthread_exit(NULL);
       }


       buffer[received] = '\0'; // Null-terminate the received message
       strcpy(cmd, buffer); // Copy the message for tokenization


       // Tokenize the command
       arguments[0] = strtok(cmd, " \n");
       i = 0;
       while (arguments[i] != NULL) {
           arguments[++i] = strtok(NULL, " \n");
       }


       // Debugging: Print the command and arguments
       printf("Command: %s, Argument: %s\n", arguments[0], arguments[1]);


       // Command Handling
       if (strcmp(arguments[0], "create") == 0) {
           printf("Processing create command for room: %s\n", arguments[1]);
           pthread_mutex_lock(&mutex);
           room_head = createRoom(room_head, arguments[1]);
           pthread_mutex_unlock(&mutex);
           sprintf(buffer, "Room '%s' created.\nchat>", arguments[1]);
           send(client, buffer, strlen(buffer), 0);
       } else if (strcmp(arguments[0], "join") == 0) {
           pthread_mutex_lock(&mutex);
           room_head = addUserToRoom(room_head, arguments[1], username);
           pthread_mutex_unlock(&mutex);
           sprintf(buffer, "Joined room '%s'.\nchat>", arguments[1]);
           send(client, buffer, strlen(buffer), 0);
       } else if (strcmp(arguments[0], "leave") == 0) {
           pthread_mutex_lock(&mutex);
           removeUserFromRoom(room_head, arguments[1], username);
           pthread_mutex_unlock(&mutex);
           sprintf(buffer, "Left room '%s'.\nchat>", arguments[1]);
           send(client, buffer, strlen(buffer), 0);
       } else if (strcmp(arguments[0], "users") == 0) {
           pthread_mutex_lock(&rw_lock);
           struct node *current = head;
           strcpy(buffer, "Connected users:\n");
           while (current) {
               strcat(buffer, current->username);
               strcat(buffer, "\n");
               current = current->next;
           }
           pthread_mutex_unlock(&rw_lock);
           strcat(buffer, "chat>");
           send(client, buffer, strlen(buffer), 0);
       } else if (strcmp(arguments[0], "rooms") == 0) {
           pthread_mutex_lock(&rw_lock);
           struct room *current = room_head;
           strcpy(buffer, "Available rooms:\n");
           while (current) {
               strcat(buffer, current->room_name);
               strcat(buffer, "\n");
               current = current->next;
           }
           pthread_mutex_unlock(&rw_lock);
           strcat(buffer, "chat>");
           send(client, buffer, strlen(buffer), 0);
       } else if (strcmp(arguments[0], "exit") == 0 || strcmp(arguments[0], "logout") == 0) {
           printf("User %s logged out.\n", username);
           pthread_mutex_lock(&mutex);
           removeUserFromRoom(room_head, DEFAULT_ROOM, username);
           pthread_mutex_unlock(&mutex);
           close(client);
           pthread_exit(NULL);
       } else {
           // Broadcast message to the default room
           sprintf(buffer, "::%s> %s\nchat>", username, cmd);
           struct room *room = findRoom(room_head, DEFAULT_ROOM);
           if (room) {
               struct node *current = room->user_list;
               while (current) {
                   if (current->socket != client) { // Don't send the message back to the sender
                       send(current->socket, buffer, strlen(buffer), 0);
                   }
                   current = current->next;
               }
           }
       }
   }
}
