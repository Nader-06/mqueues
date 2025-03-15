#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "claves.h"

// Define node structure for linked list
typedef struct Node {
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
    struct Node *next;
} Node;

// Head of the linked list
static Node *head = NULL;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Destroy all elements in the list
int destroy(void) {
    pthread_mutex_lock(&mutex);
    Node *current = head;
    while (current) {
        Node *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    pthread_mutex_unlock(&mutex);
    return 0;
}

// Insert a new key-value pair
int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > 32) return -1;
    
    pthread_mutex_lock(&mutex);
    Node *current = head;
    while (current) {
        if (current->key == key) {
            pthread_mutex_unlock(&mutex);
            return -1; // Key already exists
        }
        current = current->next;
    }
    
    Node *new_node = (Node *)malloc(sizeof(Node));
    if (!new_node) {
        pthread_mutex_unlock(&mutex);
        return -1;
    }
    new_node->key = key;
    strncpy(new_node->value1, value1, 255);
    new_node->value1[255] = '\0';
    new_node->N_value2 = N_value2;
    memcpy(new_node->V_value2, V_value2, N_value2 * sizeof(double));
    new_node->value3 = value3;
    new_node->next = head;
    head = new_node;
    
    pthread_mutex_unlock(&mutex);
    return 0;
}

// Retrieve values for a given key
int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    pthread_mutex_lock(&mutex);
    Node *current = head;
    while (current) {
        if (current->key == key) {
            strncpy(value1, current->value1, 255);
            value1[255] = '\0';
            *N_value2 = current->N_value2;
            memcpy(V_value2, current->V_value2, current->N_value2 * sizeof(double));
            *value3 = current->value3;
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex);
    return -1; // Key not found
}

// Modify existing key-value pair
int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    if (N_value2 < 1 || N_value2 > 32) return -1;
    
    pthread_mutex_lock(&mutex);
    Node *current = head;
    while (current) {
        if (current->key == key) {
            strncpy(current->value1, value1, 255);
            current->value1[255] = '\0';
            current->N_value2 = N_value2;
            memcpy(current->V_value2, V_value2, N_value2 * sizeof(double));
            current->value3 = value3;
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex);
    return -1; // Key not found
}

// Delete a key-value pair
int delete_key(int key) {
    pthread_mutex_lock(&mutex);
    Node *current = head;
    Node *prev = NULL;
    
    while (current) {
        if (current->key == key) {
            if (prev) prev->next = current->next;
            else head = current->next;
            free(current);
            pthread_mutex_unlock(&mutex);
            return 0;
        }
        prev = current;
        current = current->next;
    }
    pthread_mutex_unlock(&mutex);
    return -1; // Key not found
}

// Check if a key exists
int exist(int key) {
    pthread_mutex_lock(&mutex);
    Node *current = head;
    while (current) {
        if (current->key == key) {
            pthread_mutex_unlock(&mutex);
            return 1; // Key exists
        }
        current = current->next;
    }
    pthread_mutex_unlock(&mutex);
    return 0; // Key does not exist
}
