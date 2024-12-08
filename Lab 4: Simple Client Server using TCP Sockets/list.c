#include "list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

list_t* list_alloc() {
    list_t* list = (list_t*)malloc(sizeof(list_t));
    list->head = NULL;
    return list;
}

void list_free(list_t* list) {
    node_t* current = list->head;
    while (current) {
        node_t* temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
}

void list_add_to_back(list_t* list, int value) {
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    new_node->data = value;
    new_node->next = NULL;
    if (!list->head) {
        list->head = new_node;
        return;
    }
    node_t* current = list->head;
    while (current->next) {
        current = current->next;
    }
    current->next = new_node;
}

void list_add_to_front(list_t* list, int value) {
    node_t* new_node = (node_t*)malloc(sizeof(node_t));
    new_node->data = value;
    new_node->next = list->head;
    list->head = new_node;
}

void list_add_at_index(list_t* list, int index, int value) {
    if (index == 0) {
        list_add_to_front(list, value);
        return;
    }
    node_t* current = list->head;
    for (int i = 0; current && i < index - 1; i++) {
        current = current->next;
    }
    if (current) {
        node_t* new_node = (node_t*)malloc(sizeof(node_t));
        new_node->data = value;
        new_node->next = current->next;
        current->next = new_node;
    }
}

int list_remove_from_back(list_t* list) {
    if (!list->head) return -1;
    if (!list->head->next) {
        int value = list->head->data;
        free(list->head);
        list->head = NULL;
        return value;
    }
    node_t* current = list->head;
    while (current->next->next) {
        current = current->next;
    }
    int value = current->next->data;
    free(current->next);
    current->next = NULL;
    return value;
}

int list_remove_from_front(list_t* list) {
    if (!list->head) return -1;
    int value = list->head->data;
    node_t* temp = list->head;
    list->head = list->head->next;
    free(temp);
    return value;
}

int list_remove_at_index(list_t* list, int index) {
    if (index == 0) {
        return list_remove_from_front(list);
    }
    node_t* current = list->head;
    for (int i = 0; current && i < index - 1; i++) {
        current = current->next;
    }
    if (current && current->next) {
        node_t* temp = current->next;
        int value = temp->data;
        current->next = temp->next;
        free(temp);
        return value;
    }
    return -1;
}

int list_get_elem_at(list_t* list, int index) {
    node_t* current = list->head;
    for (int i = 0; current && i < index; i++) {
        current = current->next;
    }
    return current ? current->data : -1;
}

int list_length(list_t* list) {
    int length = 0;
    node_t* current = list->head;
    while (current) {
        length++;
        current = current->next;
    }
    return length;
}

char* listToString(list_t* list) {
    static char buffer[1024];
    buffer[0] = '\0';
    node_t* current = list->head;
    while (current) {
        char temp[16];
        sprintf(temp, "%d -> ", current->data);
        strcat(buffer, temp);
        current = current->next;
    }
    strcat(buffer, "NULL");
    return buffer;
}
