#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include "list.h"

#define PORT 9001
#define ACK "ACK"

int main(int argc, char const* argv[]) {
    int n, val, idx;
    int servSockD = socket(AF_INET, SOCK_STREAM, 0);
    char buf[1024], sbuf[1024];
    char* token;

    struct sockaddr_in servAddr;
    list_t* mylist = list_alloc();

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(PORT);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    bind(servSockD, (struct sockaddr*)&servAddr, sizeof(servAddr));
    listen(servSockD, 1);

    int clientSocket = accept(servSockD, NULL, NULL);

    while (1) {
        n = recv(clientSocket, buf, sizeof(buf), 0);
        buf[n] = '\0';

        if (n > 0) {
            token = strtok(buf, " ");
            if (strcmp(token, "exit") == 0) {
                list_free(mylist);
                exit(1);
            } else if (strcmp(token, "get_length") == 0) {
                val = list_length(mylist);
                sprintf(sbuf, "Length = %d", val);
            } else if (strcmp(token, "add_front") == 0) {
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_to_front(mylist, val);
                sprintf(sbuf, "%s %d", ACK, val);
            } else if (strcmp(token, "add_back") == 0) {
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_to_back(mylist, val);
                sprintf(sbuf, "%s %d", ACK, val);
            } else if (strcmp(token, "add_position") == 0) {
                token = strtok(NULL, " ");
                idx = atoi(token);
                token = strtok(NULL, " ");
                val = atoi(token);
                list_add_at_index(mylist, idx, val);
                sprintf(sbuf, "%s %d", ACK, val);
            } else if (strcmp(token, "remove_back") == 0) {
                val = list_remove_from_back(mylist);
                sprintf(sbuf, "Removed = %d", val);
            } else if (strcmp(token, "remove_front") == 0) {
                val = list_remove_from_front(mylist);
                sprintf(sbuf, "Removed = %d", val);
            } else if (strcmp(token, "remove_position") == 0) {
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_remove_at_index(mylist, idx);
                sprintf(sbuf, "Removed = %d", val);
            } else if (strcmp(token, "get") == 0) {
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_get_elem_at(mylist, idx);
                sprintf(sbuf, "Value = %d", val);
            } else if (strcmp(token, "print") == 0) {
                sprintf(sbuf, "%s", listToString(mylist));
            }

            send(clientSocket, sbuf, sizeof(sbuf), 0);
        }
        memset(buf, '\0', sizeof(buf));
    }
    return 0;
}
