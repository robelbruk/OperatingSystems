// list/list.c
//
// Implementation for linked list.
//
// <Author>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

list_t *list_alloc() { 
    list_t* mylist = (list_t *) malloc(sizeof(list_t)); 
    mylist->head = NULL;
    return mylist;
}

void list_free(list_t *l) {
    node_t *current = l->head;
    node_t *next_node;
    
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    free(l);
}

void list_print(list_t *l) {
    node_t *current = l->head;
    while (current != NULL) {
        printf("%d -> ", current->value);
        current = current->next;
    }
    printf("NULL\n");
}

char * listToString(list_t *l) {
    char* buf = (char *) malloc(sizeof(char) * 10024);
    char tbuf[20];

    node_t* curr = l->head;
    buf[0] = '\0';  // Initialize the string with null terminator
    while (curr != NULL) {
        sprintf(tbuf, "%d->", curr->value);
        curr = curr->next;
        strcat(buf, tbuf);
    }
    strcat(buf, "NULL");
    return buf;
}

int list_length(list_t *l) {
    int length = 0;
    node_t *current = l->head;
    while (current != NULL) {
        length++;
        current = current->next;
    }
    return length;
}

void list_add_to_back(list_t *l, elem value) {
    node_t *new_node = getNode(value);
    if (l->head == NULL) {
        l->head = new_node;
    } else {
        node_t *current = l->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = new_node;
    }
}

void list_add_to_front(list_t *l, elem value) {
    node_t *cur_node = getNode(value);
    cur_node->next = l->head;
    l->head = cur_node;
}

node_t *getNode(elem value) {
    node_t *mynode = (node_t *) malloc(sizeof(node_t));
    mynode->value = value;
    mynode->next = NULL;
    return mynode;
}

void list_add_at_index(list_t *l, elem value, int index) {
    if (index == 0) {
        list_add_to_front(l, value);
        return;
    }

    node_t *new_node = getNode(value);
    node_t *current = l->head;
    int i;
    for (i = 0; current != NULL && i < index - 1; i++) {
        current = current->next;
    }

    if (current == NULL) {
        free(new_node);
        printf("Index out of bounds!\n");
        return;
    }

    new_node->next = current->next;
    current->next = new_node;
}

elem list_remove_from_back(list_t *l) {
    if (l->head == NULL) {
        printf("Cannot remove from an empty list.\n");
        return -1;
    }

    node_t *current = l->head;
    if (current->next == NULL) { // Single element
        elem value = current->value;
        free(current);
        l->head = NULL;
        return value;
    }

    while (current->next->next != NULL) {
        current = current->next;
    }

    elem value = current->next->value;
    free(current->next);
    current->next = NULL;
    return value;
}

elem list_remove_from_front(list_t *l) {
    if (l->head == NULL) {
        printf("Cannot remove from an empty list.\n");
        return -1;
    }

    node_t *temp = l->head;
    elem value = temp->value;
    l->head = l->head->next;
    free(temp);
    return value;
}

elem list_remove_at_index(list_t *l, int index) {
    if (l->head == NULL) {
        printf("Cannot remove from an empty list.\n");
        return -1;
    }

    if (index == 0) {
        return list_remove_from_front(l);
    }

    node_t *current = l->head;
    int i;
    for (i = 0; current != NULL && i < index - 1; i++) {
        current = current->next;
    }

    if (current == NULL || current->next == NULL) {
        printf("Index out of bounds!\n");
        return -1;
    }

    node_t *temp = current->next;
    elem value = temp->value;
    current->next = temp->next;
    free(temp);

    return value;
}

bool list_is_in(list_t *l, elem value) {
    node_t *current = l->head;
    while (current != NULL) {
        if (current->value == value) return true;
        current = current->next;
    }
    return false;
}

elem list_get_elem_at(list_t *l, int index) {
    node_t *current = l->head;
    int i;
    for (i = 0; current != NULL && i < index; i++) {
        current = current->next;
    }
    if (current == NULL) return -1; // Index out of range
    return current->value;
}

int list_get_index_of(list_t *l, elem value) {
    node_t *current = l->head;
    int index = 0;
    while (current != NULL) {
        if (current->value == value) return index;
        current = current->next;
        index++;
    }
    return -1;
}
