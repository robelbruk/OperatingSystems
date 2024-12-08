#include "server.h"
#include "list.h"

#define DEFAULT_ROOM "Lobby"

// Synchronization locks and counters
extern int numReaders;
extern pthread_mutex_t rw_lock;
extern pthread_mutex_t mutex;

extern struct node *user_head;
extern struct room *room_head;
extern char *server_MOTD;

/* Thread for handling client communication */
void *client_receive(void *ptr) {
    int client = *(int *)ptr;
    char buffer[MAXBUFF], cmd[MAXBUFF], tmpbuf[MAXBUFF];
    char *arguments[80];
    int received, i;

    // Add guest user
    char username[30];
    sprintf(username, "guest%d", client);
    pthread_mutex_lock(&mutex);
    user_head = insertFirstU(user_head, client, username);
    pthread_mutex_unlock(&mutex);

    // Add user to default room
    struct room *lobby = findR(room_head, DEFAULT_ROOM);
    if (lobby) {
        pthread_mutex_lock(&mutex);
        addUserToRoom(lobby, findU(user_head, username));
        pthread_mutex_unlock(&mutex);
    }

    // Send MOTD
    send(client, server_MOTD, strlen(server_MOTD), 0);

    while (1) {
        if ((received = read(client, buffer, MAXBUFF)) != 0) {
            buffer[received] = '\0';
            strcpy(cmd, buffer);

            // Parse command
            arguments[0] = strtok(cmd, " ");
            i = 0;
            while (arguments[i] != NULL) {
                arguments[++i] = strtok(NULL, " ");
            }

            // Handle commands
            if (strcmp(arguments[0], "create") == 0) {
                pthread_mutex_lock(&mutex);
                room_head = insertFirstR(room_head, arguments[1]);
                pthread_mutex_unlock(&mutex);
                sprintf(tmpbuf, "Room %s created.\nchat>", arguments[1]);
                send(client, tmpbuf, strlen(tmpbuf), 0);
            } else if (strcmp(arguments[0], "join") == 0) {
                struct room *room = findR(room_head, arguments[1]);
                if (room) {
                    pthread_mutex_lock(&mutex);
                    addUserToRoom(room, findU(user_head, username));
                    pthread_mutex_unlock(&mutex);
                    sprintf(tmpbuf, "Joined room %s.\nchat>", arguments[1]);
                    send(client, tmpbuf, strlen(tmpbuf), 0);
                } else {
                    sprintf(tmpbuf, "Room %s does not exist.\nchat>", arguments[1]);
                    send(client, tmpbuf, strlen(tmpbuf), 0);
                }
            } else if (strcmp(arguments[0], "leave") == 0) {
                struct room *room = findR(room_head, arguments[1]);
                if (room) {
                    pthread_mutex_lock(&mutex);
                    removeUserFromRoom(room, findU(user_head, username));
                    pthread_mutex_unlock(&mutex);
                    sprintf(tmpbuf, "Left room %s.\nchat>", arguments[1]);
                    send(client, tmpbuf, strlen(tmpbuf), 0);
                } else {
                    sprintf(tmpbuf, "Room %s does not exist.\nchat>", arguments[1]);
                    send(client, tmpbuf, strlen(tmpbuf), 0);
                }
            } else if (strcmp(arguments[0], "rooms") == 0) {
                listRooms(room_head, client);
            } else if (strcmp(arguments[0], "users") == 0) {
                listUsers(user_head, client);
            } else if (strcmp(arguments[0], "login") == 0) {
                pthread_mutex_lock(&mutex);
                struct node *user = findU(user_head, username);
                if (user) {
                    strcpy(user->username, arguments[1]);
                }
                pthread_mutex_unlock(&mutex);
                sprintf(tmpbuf, "Logged in as %s.\nchat>", arguments[1]);
                send(client, tmpbuf, strlen(tmpbuf), 0);
            } else if (strcmp(arguments[0], "connect") == 0) {
                // Handle DM logic here
                sprintf(tmpbuf, "Connected to %s.\nchat>", arguments[1]);
                send(client, tmpbuf, strlen(tmpbuf), 0);
            } else if (strcmp(arguments[0], "disconnect") == 0) {
                // Handle disconnect logic here
                sprintf(tmpbuf, "Disconnected from %s.\nchat>", arguments[1]);
                send(client, tmpbuf, strlen(tmpbuf), 0);
            } else if (strcmp(arguments[0], "exit") == 0 || strcmp(arguments[0], "logout") == 0) {
                pthread_mutex_lock(&mutex);
                user_head = removeUser(user_head, client);
                pthread_mutex_unlock(&mutex);
                close(client);
                return NULL;
            } else {
                sprintf(tmpbuf, "Unknown command.\nchat>");
                send(client, tmpbuf, strlen(tmpbuf), 0);
            }

            memset(buffer, 0, sizeof(buffer));
        }
    }
    return NULL;
}
