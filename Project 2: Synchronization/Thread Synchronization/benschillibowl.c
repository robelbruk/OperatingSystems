#include "BENSCHILLIBOWL.h"

#include <assert.h>
#include <stdlib.h>
#include <time.h>

bool IsEmpty(BENSCHILLIBOWL* bcb);
bool IsFull(BENSCHILLIBOWL* bcb);
void AddOrderToBack(Order **orders, Order *order);

/* Select a random item from the Menu and return it */
MenuItem PickRandomMenuItem() {
    return BENSCHILLIBOWLMenu[rand() % BENSCHILLIBOWLMenuLength];
}

/* Allocate memory for the Restaurant, then create the mutex and condition variables needed to instantiate the Restaurant */
BENSCHILLIBOWL* OpenRestaurant(int max_size, int expected_num_orders) {
    BENSCHILLIBOWL* bcb = (BENSCHILLIBOWL*)malloc(sizeof(BENSCHILLIBOWL));
    if (!bcb) {
        perror("Failed to allocate memory for restaurant");
        exit(1);
    }

    bcb->orders = NULL;
    bcb->current_size = 0;
    bcb->max_size = max_size;
    bcb->next_order_number = 1;
    bcb->orders_handled = 0;
    bcb->expected_num_orders = expected_num_orders;

    pthread_mutex_init(&bcb->mutex, NULL);
    pthread_cond_init(&bcb->can_add_orders, NULL);
    pthread_cond_init(&bcb->can_get_orders, NULL);

    printf("Restaurant is open!\n");
    return bcb;
}

/* Check that the number of orders received is equal to the number handled (ie. fulfilled).
   Remember to deallocate your resources */
void CloseRestaurant(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&bcb->mutex);

    if (bcb->orders_handled != bcb->expected_num_orders) {
        printf("Error: Not all orders handled. Orders handled: %d, Expected: %d\n",
               bcb->orders_handled, bcb->expected_num_orders);
    } else {
        printf("All orders successfully handled.\n");
    }

    pthread_mutex_unlock(&bcb->mutex);

    pthread_mutex_destroy(&bcb->mutex);
    pthread_cond_destroy(&bcb->can_add_orders);
    pthread_cond_destroy(&bcb->can_get_orders);

    free(bcb);
    printf("Restaurant is closed!\n");
}

/* Add an order to the back of the queue */
int AddOrder(BENSCHILLIBOWL* bcb, Order* order) {
    pthread_mutex_lock(&bcb->mutex);

    // Wait until the queue is not full
    while (IsFull(bcb)) {
        pthread_cond_wait(&bcb->can_add_orders, &bcb->mutex);
    }

    order->order_number = bcb->next_order_number++;
    AddOrderToBack(&bcb->orders, order);
    bcb->current_size++;

    pthread_cond_signal(&bcb->can_get_orders); // Notify cooks that an order is available
    pthread_mutex_unlock(&bcb->mutex);

    return order->order_number;
}

/* Remove an order from the queue */
Order* GetOrder(BENSCHILLIBOWL* bcb) {
    pthread_mutex_lock(&bcb->mutex);

    // Wait until the queue is not empty
    while (IsEmpty(bcb)) {
        if (bcb->orders_handled == bcb->expected_num_orders) {
            pthread_mutex_unlock(&bcb->mutex);
            return NULL; // All orders have been handled
        }
        pthread_cond_wait(&bcb->can_get_orders, &bcb->mutex);
    }

    Order* order = bcb->orders;
    bcb->orders = bcb->orders->next;
    bcb->current_size--;
    bcb->orders_handled++;

    pthread_cond_signal(&bcb->can_add_orders); // Notify customers that space is available
    pthread_mutex_unlock(&bcb->mutex);

    return order;
}

/* Optional helper function to check if the queue is full */
bool IsFull(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == bcb->max_size;
}

/* Optional helper function to check if the queue is empty */
bool IsEmpty(BENSCHILLIBOWL* bcb) {
    return bcb->current_size == 0;
}

/* This method adds an order to the rear of the queue */
void AddOrderToBack(Order **orders, Order *order) {
    if (*orders == NULL) {
        *orders = order;
        order->next = NULL;
    } else {
        Order* temp = *orders;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = order;
        order->next = NULL;
    }
}
