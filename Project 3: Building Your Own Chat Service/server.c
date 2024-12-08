#include "server.h"
#include "list.h"

int chat_serv_sock_fd;  // Server socket
struct node *user_head = NULL;  // Head of user list
struct room *room_head = NULL;  // Head of room list

/////////////////////////////////////////////
// Synchronization locks and counters
int numReaders = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t rw_lock = PTHREAD_MUTEX_INITIALIZER;
/////////////////////////////////////////////

const char *server_MOTD = "Welcome to BisonChat Server.\nType 'help' for commands.\nchat>";

/* Main function */
int main(int argc, char **argv) {
    signal(SIGINT, sigintHandler);

    // Create the default room for all clients
    room_head = insertFirstR(room_head, DEFAULT_ROOM);

    // Open server socket
    chat_serv_sock_fd = get_server_socket();

    // Start server
    if (start_server(chat_serv_sock_fd, BACKLOG) == -1) {
        perror("start_server error");
        exit(1);
    }

    printf("Server Launched! Listening on PORT: %d\n", PORT);

    // Main loop to accept connections
    while (1) {
        int new_client = accept_client(chat_serv_sock_fd);
        if (new_client != -1) {
            pthread_t new_client_thread;
            pthread_create(&new_client_thread, NULL, client_receive, (void *)&new_client);
        }
    }

    close(chat_serv_sock_fd);
    return 0;
}

/* Create server socket */
int get_server_socket() {
    int opt = TRUE;
    int master_socket;
    struct sockaddr_in address;

    // Create master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set options for multiple connections
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Configure socket address
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    return master_socket;
}

/* Start server */
int start_server(int serv_socket, int backlog) {
    int status = listen(serv_socket, backlog);
    if (status == -1) {
        perror("listen error");
    }
    return status;
}

/* Accept a client */
int accept_client(int serv_sock) {
    int client_sock;
    socklen_t addr_size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage client_addr;

    if ((client_sock = accept(serv_sock, (struct sockaddr *)&client_addr, &addr_size)) == -1) {
        perror("accept error");
    }
    return client_sock;
}

/* Handle SIGINT (Ctrl+C) */
void sigintHandler(int sig_num) {
    printf("\nServer shutting down...\n");

    // Notify connected users
    struct node *current = user_head;
    char shutdown_msg[] = "Server is shutting down. Goodbye!\n";
    while (current) {
        send(current->socket, shutdown_msg, strlen(shutdown_msg), 0);
        close(current->socket);
        current = current->next;
    }

    // Free memory for users and rooms
    while (user_head) {
        user_head = removeUser(user_head, user_head->socket);
    }
    while (room_head) {
        room_head = removeRoom(room_head, room_head->name);
    }

    // Close server socket
    close(chat_serv_sock_fd);
    printf("Server closed gracefully.\n");
    exit(0);
}
