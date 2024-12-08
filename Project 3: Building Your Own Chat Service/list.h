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

// Room node structure
struct room {
   char name[50];
   struct node *users;  // Linked list of users in the room
   struct room *next;
};

/////////////////// USERLIST //////////////////////////
// Insert node at the first location
struct node* insertFirstU(struct node *head, int socket, char *username);

// Find a node with the given username
struct node* findU(struct node *head, char *username);

// Remove a user from the list
struct node* removeUser(struct node *head, int socket);

// List all users in the linked list
void listUsers(struct node *head, int client_socket);

/////////////////// ROOMLIST //////////////////////////
// Insert a room at the first location
struct room* insertFirstR(struct room *head, char *room_name);

// Find a room with the given name
struct room* findR(struct room *head, char *room_name);

// Remove a room from the list
struct room* removeRoom(struct room *head, char *room_name);

// List all rooms
void listRooms(struct room *head, int client_socket);

// Add a user to a room
void addUserToRoom(struct room *room, struct node *user);

// Remove a user from a room
void removeUserFromRoom(struct room *room, struct node *user);

#endif
