#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "list.h"

// Insert node at the first location
struct node* insertFirstU(struct node *head, int socket, char *username) {
   if (findU(head, username) == NULL) {
       // Create a new user node
       struct node *link = (struct node *)malloc(sizeof(struct node));
       link->socket = socket;
       strcpy(link->username, username);

       // Point to old first node
       link->next = head;

       // Return new head
       head = link;
   } else {
       printf("Duplicate: %s\n", username);
   }
   return head;
}

// Find a node with the given username
struct node* findU(struct node *head, char *username) {
   struct node *current = head;

   // Navigate through list
   while (current != NULL) {
       if (strcmp(current->username, username) == 0) {
           return current;  // Found user
       }
       current = current->next;
   }
   return NULL;  // User not found
}

// Remove a user by socket
struct node* removeUser(struct node *head, int socket) {
   struct node *current = head;
   struct node *prev = NULL;

   while (current != NULL) {
       if (current->socket == socket) {
           if (prev == NULL) {  // Removing head node
               head = current->next;
           } else {
               prev->next = current->next;
           }
           free(current);
           return head;
       }
       prev = current;
       current = current->next;
   }
   return head;  // User not found
}

// List all users
void listUsers(struct node *head, int client_socket) {
   char buffer[1024] = "Users:\n";
   struct node *current = head;

   while (current != NULL) {
       strcat(buffer, current->username);
       strcat(buffer, "\n");
       current = current->next;
   }
   send(client_socket, buffer, strlen(buffer), 0);
}

// Insert a room at the first location
struct room* insertFirstR(struct room *head, char *room_name) {
   if (findR(head, room_name) == NULL) {
       // Create a new room node
       struct room *link = (struct room *)malloc(sizeof(struct room));
       strcpy(link->name, room_name);
       link->users = NULL;

       // Point to old first node
       link->next = head;

       // Return new head
       head = link;
   } else {
       printf("Duplicate room: %s\n", room_name);
   }
   return head;
}

// Find a room with the given name
struct room* findR(struct room *head, char *room_name) {
   struct room *current = head;

   // Navigate through list
   while (current != NULL) {
       if (strcmp(current->name, room_name) == 0) {
           return current;  // Found room
       }
       current = current->next;
   }
   return NULL;  // Room not found
}

// Remove a room by name
struct room* removeRoom(struct room *head, char *room_name) {
   struct room *current = head;
   struct room *prev = NULL;

   while (current != NULL) {
       if (strcmp(current->name, room_name) == 0) {
           if (prev == NULL) {  // Removing head node
               head = current->next;
           } else {
               prev->next = current->next;
           }
           free(current);
           return head;
       }
       prev = current;
       current = current->next;
   }
   return head;  // Room not found
}

// List all rooms
void listRooms(struct room *head, int client_socket) {
   char buffer[1024] = "Rooms:\n";
   struct room *current = head;

   while (current != NULL) {
       strcat(buffer, current->name);
       strcat(buffer, "\n");
       current = current->next;
   }
   send(client_socket, buffer, strlen(buffer), 0);
}

// Add a user to a room
void addUserToRoom(struct room *room, struct node *user) {
   user->next = room->users;
   room->users = user;
}

// Remove a user from a room
void removeUserFromRoom(struct room *room, struct node *user) {
   struct node *current = room->users;
   struct node *prev = NULL;

   while (current != NULL) {
       if (current == user) {
           if (prev == NULL) {  // Removing head node
               room->users = current->next;
           } else {
               prev->next = current->next;
           }
           return;
       }
       prev = current;
       current = current->next;
   }
}
