#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "claves.h"

#define MAX_MSG_SIZE 1024
#define MQ_NAME "/mq_servidor"
#define MQ_RESPONSE "/mq_cliente"

// Function to process client requests **sequentially**
void handle_request(char *buffer) {
    char response[MAX_MSG_SIZE];

    // Command and variables
    char command[10];
    int key;
    char value1[256];
    int N_value2;
    double value2[32];
    struct Coord value3;

    printf("Processing request: %s\n", buffer);

    if (sscanf(buffer, "%s %d %255s %d", command, &key, value1, &N_value2) >= 2) {
        if (strcmp(command, "SET") == 0) {
            printf("User called SET\n");
            int result = set_value(key, value1, N_value2, value2, value3);
            snprintf(response, sizeof(response), "%d", result);
        } 
        else if (strcmp(command, "GET") == 0) {
            printf("User called GET\n");
            int result = get_value(key, value1, &N_value2, value2, &value3);
            snprintf(response, sizeof(response), "%d %s %d", result, value1, N_value2);
        } 
        else if (strcmp(command, "MODIFY") == 0) {
            printf("User called MODIFY\n");
            int result = modify_value(key, value1, N_value2, value2, value3);
            snprintf(response, sizeof(response), "%d", result);
        } 
        else if (strcmp(command, "DELETE") == 0) {
            printf("User called DELETE\n");
            int result = delete_key(key);
            snprintf(response, sizeof(response), "%d", result);
        } 
        else if (strcmp(command, "EXIST") == 0) {
            printf("User called EXIST\n");
            int result = exist(key);
            snprintf(response, sizeof(response), "%d", result);
        } 
        else if (strcmp(command, "DESTROY") == 0) {
            printf("User called DESTROY\n");
            int result = destroy();
            snprintf(response, sizeof(response), "%d", result);
        } 
        else {
            printf("Invalid command\n");
            snprintf(response, sizeof(response), "-1");
        }
    } else {
        printf("Malformed request\n");
        snprintf(response, sizeof(response), "-1");
    }

    // Send response back to client via MQ
    mqd_t mq_client = mq_open(MQ_RESPONSE, O_WRONLY);
    if (mq_client == (mqd_t)-1) {
        perror("mq_open (client response)");
    } else {
        mq_send(mq_client, response, strlen(response) + 1, 0);
        mq_close(mq_client);
    }
}

int main(int argc, char *argv[]) {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    mq = mq_open(MQ_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    printf("Servidor de mensajes iniciado (Sequential Processing)\n");

    while (1) {
        memset(buffer, 0, MAX_MSG_SIZE);

        ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read == -1) {
            perror("mq_receive");
            continue;
        }

        if (strcmp(buffer, "EXIT") == 0) {
            printf("Server shutting down.\n");
            break;
        }

        // Directly process the request in the main loop
        handle_request(buffer);
    }

    printf("Cerrando servidor\n");
    mq_close(mq);
    mq_unlink(MQ_NAME);
    return 0;
}
