#include "server.h"


int chat_serv_sock_fd; // Server socket
int numReaders = 0;    // Reader count
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER;


struct node *head = NULL;       // User list
struct room *room_head = NULL;  // Room list


int main(int argc, char **argv) {
   signal(SIGINT, sigintHandler);


   // Initialize the default room
   room_head = createRoom(room_head, DEFAULT_ROOM);


   // Create and bind server socket
   chat_serv_sock_fd = get_server_socket();
   if (start_server(chat_serv_sock_fd, BACKLOG) == -1) {
       perror("Server start error");
       exit(1);
   }


   printf("Server launched on PORT %d\n", PORT);


   // Main loop
   while (1) {
       int new_client = accept_client(chat_serv_sock_fd);
       if (new_client != -1) {
           pthread_t client_thread;
           pthread_create(&client_thread, NULL, client_receive, (void *)&new_client);
       }
   }


   close(chat_serv_sock_fd);
}


int get_server_socket() {
   int opt = TRUE;
   int server_socket = socket(AF_INET, SOCK_STREAM, 0);
   if (server_socket == 0) {
       perror("Socket failed");
       exit(EXIT_FAILURE);
   }


   setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));


   struct sockaddr_in address = {
       .sin_family = AF_INET,
       .sin_addr.s_addr = INADDR_ANY,
       .sin_port = htons(PORT)
   };


   if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
       perror("Bind failed");
       exit(EXIT_FAILURE);
   }


   return server_socket;
}


int start_server(int serv_socket, int backlog) {
   return listen(serv_socket, backlog);
}


int accept_client(int serv_sock) {
   struct sockaddr_storage client_addr;
   socklen_t addr_size = sizeof(client_addr);
   return accept(serv_sock, (struct sockaddr *)&client_addr, &addr_size);
}


void sigintHandler(int sig_num) {
   printf("\nShutting down server...\n");


   pthread_mutex_lock(&mutex);
   struct node *current = head, *temp;
   while (current) {
       close(current->socket);
       temp = current;
       current = current->next;
       free(temp);
   }
   head = NULL;


   struct room *room_current = room_head, *room_temp;
   while (room_current) {
       room_temp = room_current;
       room_current = room_current->next;
       free(room_temp);
   }
   room_head = NULL;
   pthread_mutex_unlock(&mutex);


   close(chat_serv_sock_fd);
   exit(0);
}