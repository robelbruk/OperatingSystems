#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "BENSCHILLIBOWL.h"

#define NUM_CUSTOMERS 10
#define NUM_COOKS 3
#define RESTAURANT_MAX_SIZE 5

BENSCHILLIBOWL *bcb; // Global restaurant variable

void* BENSCHILLIBOWLCustomer(void* tid) {
    int customer_id = *(int*)tid;
    free(tid); // Free allocated memory for customer_id
    printf("Customer %d: Entering the restaurant.\n", customer_id);

    Order *order = (Order*)malloc(sizeof(Order));
    if (!order) {
        perror("Failed to allocate memory for order");
        return NULL;
    }

    order->menu_item = PickRandomMenuItem();
    order->customer_id = customer_id;

    int order_number = AddOrder(bcb, order);
    if (order_number != -1) {
        printf("Customer %d: Placed Order #%d for %s.\n", customer_id, order_number, order->menu_item);
    } else {
        printf("Customer %d: Failed to place an order.\n", customer_id);
    }

    return NULL;
}

void* BENSCHILLIBOWLCook(void* tid) {
    int cook_id = *(int*)tid;
    free(tid); // Free allocated memory for cook_id
    printf("Cook %d: Starting shift.\n", cook_id);

    while (1) {
        Order *order = GetOrder(bcb);
        if (!order) {
            printf("Cook %d: No more orders to fulfill. Ending shift.\n", cook_id);
            break;
        }

        printf("Cook %d: Fulfilled Order #%d for Customer %d (%s).\n",
               cook_id, order->order_number, order->customer_id, order->menu_item);

        free(order); // Free memory allocated for the order
    }

    return NULL;
}

int main() {
    printf("Welcome to BensChilliBowl!\n");

    // Open the restaurant
    bcb = OpenRestaurant(RESTAURANT_MAX_SIZE, NUM_CUSTOMERS);

    // Create customer and cook threads
    pthread_t customers[NUM_CUSTOMERS];
    pthread_t cooks[NUM_COOKS];

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        int *customer_id = malloc(sizeof(int));
        if (!customer_id) {
            perror("Failed to allocate memory for customer_id");
            exit(1);
        }
        *customer_id = i + 1;
        pthread_create(&customers[i], NULL, BENSCHILLIBOWLCustomer, customer_id);
    }

    for (int i = 0; i < NUM_COOKS; i++) {
        int *cook_id = malloc(sizeof(int));
        if (!cook_id) {
            perror("Failed to allocate memory for cook_id");
            exit(1);
        }
        *cook_id = i + 1;
        pthread_create(&cooks[i], NULL, BENSCHILLIBOWLCook, cook_id);
    }

    // Wait for all customers to finish
    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customers[i], NULL);
    }

    // Close the restaurant once all orders are placed
    CloseRestaurant(bcb);

    // Wait for all cooks to finish
    for (int i = 0; i < NUM_COOKS; i++) {
        pthread_join(cooks[i], NULL);
    }

    printf("BensChilliBowl is now closed. Goodbye!\n");
    return 0;
}
