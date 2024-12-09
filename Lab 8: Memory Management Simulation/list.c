#include "list.h"
#include <stdlib.h>

bool list_is_in_by_pid(list_t *list, int pid) {
    node_t *current = list->head;
    while (current) {
        if (current->blk->pid == pid) {
            return true;
        }
        current = current->next;
    }
    return false;
}

void list_add_ascending_by_blocksize(list_t *list, block_t *blk) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->blk = blk;
    new_node->next = NULL;

    if (!list->head || (blk->end - blk->start + 1) < (list->head->blk->end - list->head->blk->start + 1)) {
        new_node->next = list->head;
        list->head = new_node;
        return;
    }

    node_t *current = list->head;
    while (current->next && (blk->end - blk->start + 1) >= (current->next->blk->end - current->next->blk->start + 1)) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
}

void list_add_ascending_by_address(list_t *list, block_t *blk) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->blk = blk;
    new_node->next = NULL;

    if (!list->head || blk->start < list->head->blk->start) {
        new_node->next = list->head;
        list->head = new_node;
        return;
    }

    node_t *current = list->head;
    while (current->next && blk->start >= current->next->blk->start) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
}

void list_add_descending_by_blocksize(list_t *list, block_t *blk) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->blk = blk;
    new_node->next = NULL;

    if (!list->head || (blk->end - blk->start + 1) > (list->head->blk->end - list->head->blk->start + 1)) {
        new_node->next = list->head;
        list->head = new_node;
        return;
    }

    node_t *current = list->head;
    while (current->next && (blk->end - blk->start + 1) <= (current->next->blk->end - current->next->blk->start + 1)) {
        current = current->next;
    }

    new_node->next = current->next;
    current->next = new_node;
}

void list_coalese_nodes(list_t *list) {
    node_t *current = list->head;
    while (current && current->next) {
        if (current->blk->end + 1 == current->next->blk->start) {
            current->blk->end = current->next->blk->end;
            node_t *temp = current->next;
            current->next = temp->next;
            free(temp->blk);
            free(temp);
        } else {
            current = current->next;
        }
    }
}
