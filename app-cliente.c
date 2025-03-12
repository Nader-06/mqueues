#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "claves.h"

int main() {
    printf("Cliente de mensajes iniciado\n");

    int key = 1;
    char value1[256] = "Hello from Client";
    double value2[3] = {1.1, 2.2, 3.3};
    struct Coord value3 = {10, 20};

    // Test SET
    if (set_value(key, value1, 3, value2, value3) == 0) {
        printf("✅ SET successful\n");
    } else {
        printf("❌ SET failed\n");
    }

    // Test GET
    char retrieved_value[256];
    int N_value2;
    double v2_out[32];
    struct Coord v3_out;

    if (get_value(key, retrieved_value, &N_value2, v2_out, &v3_out) == 0) {
        printf("✅ GET successful: %s, N_value2: %d, Coord: (%d, %d)\n",
               retrieved_value, N_value2, v3_out.x, v3_out.y);
    } else {
        printf("❌ GET failed\n");
    }

    // Test MODIFY
    char new_value1[256] = "Modified Value";
    double new_value2[3] = {9.9, 8.8, 7.7};
    struct Coord new_value3 = {50, 60};

    if (modify_value(key, new_value1, 3, new_value2, new_value3) == 0) {
        printf("✅ MODIFY successful\n");
    } else {
        printf("❌ MODIFY failed\n");
    }

    // Test EXIST
    if (exist(key) == 1) {
        printf("✅ EXIST: Key exists\n");
    } else {
        printf("❌ EXIST: Key does not exist\n");
    }

    // Test DELETE
    if (delete_key(key) == 0) {
        printf("✅ DELETE successful\n");
    } else {
        printf("❌ DELETE failed\n");
    }

    // Test DESTROY
    if (destroy() == 0) {
        printf("✅ DESTROY successful\n");
    } else {
        printf("❌ DESTROY failed\n");
    }

    return 0;
}
