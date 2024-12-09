#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "util.h"

void allocate_memory(list_t *freelist, list_t *alloclist, int pid, int blocksize, int policy) {
    node_t *current = freelist->head;
    node_t *best_fit = NULL;
    node_t *worst_fit = NULL;

    // Find the block based on the policy
    while (current) {
        int block_size = current->blk->end - current->blk->start + 1;
        if (block_size >= blocksize) {
            if (policy == 1) { // First Fit
                best_fit = current;
                break;
            } else if (policy == 2) { // Best Fit
                if (!best_fit || block_size < (best_fit->blk->end - best_fit->blk->start + 1)) {
                    best_fit = current;
                }
            } else if (policy == 3) { // Worst Fit
                if (!worst_fit || block_size > (worst_fit->blk->end - worst_fit->blk->start + 1)) {
                    worst_fit = current;
                }
            }
        }
        current = current->next;
    }

    node_t *selected = (policy == 3) ? worst_fit : best_fit;

    if (!selected) {
        printf("Error: Not Enough Memory\n");
        return;
    }

    block_t *allocated_block = selected->blk;

    // Remove selected block from the freelist
    list_remove_by_reference(freelist, selected);

    // Create a new allocated block
    block_t *new_block = malloc(sizeof(block_t));
    new_block->pid = pid;
    new_block->start = allocated_block->start;
    new_block->end = new_block->start + blocksize - 1;

    // Adjust remaining free block (fragment)
    allocated_block->start = new_block->end + 1;

    // If the fragment is empty, discard it
    if (allocated_block->start > allocated_block->end) {
        free(allocated_block);
    } else {
        if (policy == 1) { // FIFO
            list_add_to_back(freelist, allocated_block);
        } else if (policy == 2) { // Best Fit
            list_add_ascending_by_blocksize(freelist, allocated_block);
        } else if (policy == 3) { // Worst Fit
            list_add_descending_by_blocksize(freelist, allocated_block);
        }
    }

    // Add new allocated block to alloclist
    list_add_ascending_by_address(alloclist, new_block);
}

void deallocate_memory(list_t *alloclist, list_t *freelist, int pid, int policy) {
    node_t *current = alloclist->head;

    // Find the block to deallocate
    while (current) {
        if (current->blk->pid == pid) {
            break;
        }
        current = current->next;
    }

    if (!current) {
        printf("Error: Can't locate Memory Used by PID: %d\n", pid);
        return;
    }

    block_t *block_to_free = current->blk;

    // Remove block from alloclist
    list_remove_by_reference(alloclist, current);

    // Reset the PID to 0
    block_to_free->pid = 0;

    // Add block back to freelist based on policy
    if (policy == 1) { // FIFO
        list_add_to_back(freelist, block_to_free);
    } else if (policy == 2) { // Best Fit
        list_add_ascending_by_blocksize(freelist, block_to_free);
    } else if (policy == 3) { // Worst Fit
        list_add_descending_by_blocksize(freelist, block_to_free);
    }
}
