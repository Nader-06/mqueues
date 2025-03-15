#include <stdio.h>
#include <stdlib.h>
#include "claves.h"

int main() {
    int key = 1;
    char value1[256] = "Test Value";
    double V_value2[3] = {1.1, 2.2, 3.3};
    struct Coord value3 = {10, 20};
    int N_value2 = 3;

    printf("Client: Setting value for key %d\n", key);
    if (set_value(key, value1, N_value2, V_value2, value3) == 0) {
        printf("Client: Value set successfully.\n");
    } else {
        printf("Client: Error setting value.\n");
    }

    printf("Client: Retrieving value for key %d\n", key);
    char retrieved_value1[256];
    double retrieved_V_value2[32];
    struct Coord retrieved_value3;
    int retrieved_N_value2;

    if (get_value(key, retrieved_value1, &retrieved_N_value2, retrieved_V_value2, &retrieved_value3) == 0) {
        printf("Client: Retrieved value1: %s\n", retrieved_value1);
        printf("Client: Retrieved N_value2: %d\n", retrieved_N_value2);
        printf("Client: Retrieved value3: (%d, %d)\n", retrieved_value3.x, retrieved_value3.y);
    } else {
        printf("Client: Error retrieving value.\n");
    }

    printf("Client: Modifying value for key %d\n", key);
    char new_value1[256] = "Updated Value";
    double new_V_value2[2] = {4.4, 5.5};
    struct Coord new_value3 = {30, 40};
    int new_N_value2 = 2;

    if (modify_value(key, new_value1, new_N_value2, new_V_value2, new_value3) == 0) {
        printf("Client: Value modified successfully.\n");
    } else {
        printf("Client: Error modifying value.\n");
    }

    printf("Client: Checking if key %d exists\n", key);
    int exists = exist(key);
    if (exists == 1) {
        printf("Client: Key %d exists.\n", key);
    } else if (exists == 0) {
        printf("Client: Key %d does not exist.\n", key);
    } else {
        printf("Client: Error checking key existence.\n");
    }

    printf("Client: Deleting key %d\n", key);
    if (delete_key(key) == 0) {
        printf("Client: Key deleted successfully.\n");
    } else {
        printf("Client: Error deleting key.\n");
    }

    printf("Client: Destroying all keys\n");
    if (destroy() == 0) {
        printf("Client: All keys destroyed successfully.\n");
    } else {
        printf("Client: Error destroying keys.\n");
    }
    
    return 0;
}
