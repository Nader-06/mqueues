#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <pthread.h>
#include "claves.h"

#define MAX_MSG_SIZE 1024
#define MQ_NAME "/mq_servidor"

//Struct to pass multiple arguments to the thread
typedef struct {
    char message[MAX_MSG_SIZE];
    int client_id;
} ThreadData;

//Thread function (matches your structure)
void *handle_requests(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    char buffer[MAX_MSG_SIZE];
    strcpy(buffer, data->message);  // Copy message to buffer
    int client_id = data->client_id;  // Store client ID
    free(data);  // Free memory after copying

    fprintf(stderr,"Processing request from client %d\n", client_id);

    if (strcmp(buffer, "DESTROY") == 0) {
        printf("User called DESTROY\n");
    } 
    else if (strcmp(buffer, "SET") == 0) {
        printf("User called SET\n");
        set_value(1, "Hello, World!", 3, (double[]){1.1, 2.2, 3.3}, (struct Coord){10, 20});
    } 
    else if (strcmp(buffer, "GET") == 0) {
        printf("User called GET\n");
        char retrieved_value[256];
        int N_value2;
        double v2_out[32];
        struct Coord v3_out;
        get_value(1, retrieved_value, &N_value2, v2_out, &v3_out);
        printf("Retrieved: %s, N_value2: %d, Coord: (%d, %d)\n", retrieved_value, N_value2, v3_out.x, v3_out.y);
    } 
    else if (strcmp(buffer, "MODIFY") == 0) {
        printf("User called MODIFY\n");
        modify_value(1, "Modified Value", 3, (double[]){9.9, 8.8, 7.7}, (struct Coord){50, 60});
    } 
    else if (strcmp(buffer, "DELETE") == 0) {
        printf("User called DELETE\n");
        delete_key(1);
    } 
    else if (strcmp(buffer, "EXIST") == 0) {
        printf("User called EXIST\n");
        exist(1);
    } 
    else {
        printf("Invalid command\n");
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    
    //Message Queue attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    //Open the message queue
    mq = mq_open(MQ_NAME, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    printf("Servidor de mensajes iniciado hello world\n");

    int client_id = 0;  // Simulated client ID counter

    while (1) {
        char *buffer = malloc(MAX_MSG_SIZE);
        if (!buffer) {
            perror("malloc");
            exit(1);
        }
        memset(buffer, 0, MAX_MSG_SIZE);

        //Receive message from client
        ssize_t bytes_read = mq_receive(mq, buffer, MAX_MSG_SIZE, NULL);
        if (bytes_read == -1) {
            perror("mq_receive");
            free(buffer);
            continue;
        }

        //Stop server if "EXIT" command is received
        if (strcmp(buffer, "EXIT") == 0) {
            printf("Server shutting down.\n");
            free(buffer);
            break;
        }

        //Allocate memory for the thread data
        ThreadData *data = malloc(sizeof(ThreadData));
        if (!data) {
            perror("malloc");
            free(buffer);
            continue;
        }

        strcpy(data->message, buffer);
        data->client_id = client_id++;
        free(buffer); 

        //new thread to handle the request
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_requests, data) != 0) {
            perror("pthread_create");
            free(data);
        } else {
            pthread_detach(thread); 
        }
    }

    //Close and clean up the message queue
    printf("Cerrando servidor\n");
    mq_close(mq);
    mq_unlink(MQ_NAME);

    return 0;
}
