#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <mqueue.h>
#include <sys/stat.h>
#include "claves.h"

#define MQ_NAME "/mq_servidor"
#define MQ_RESPONSE "/mq_cliente"
#define MAX_MSG_SIZE 1024

// Message Queue Descriptors
mqd_t mq_server, mq_client;

// Initialize message queues
void init_mq() {
    // Open the server queue (WRITE)
    mq_server = mq_open(MQ_NAME, O_WRONLY);
    if (mq_server == (mqd_t)-1) {
        perror("mq_open (server) FAILED");
        exit(1);
    }

    // Define attributes for the client queue
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    // Open the client response queue (READ)
    mq_client = mq_open(MQ_RESPONSE, O_RDONLY | O_CREAT, 0644, &attr);
    if (mq_client == (mqd_t)-1) {
        perror("mq_open (client) FAILED");
        exit(1);
    }
}

// Close message queues
void close_mq() {
    mq_close(mq_server);
    mq_close(mq_client);
}

// Send a request to the server and wait for a response
int send_request(const char *request) {
    char response[MAX_MSG_SIZE];

    // Send request to the server
    if (mq_send(mq_server, request, strlen(request) + 1, 0) == -1) {
        perror("mq_send");
        return -2;  // Communication error
    }

    // Receive response from the server
    if (mq_receive(mq_client, response, MAX_MSG_SIZE, NULL) == -1) {
        perror("mq_receive");
        return -2;
    }

    return atoi(response);  // Convert response to integer and return
}

// API Functions
int destroy() {
    init_mq();
    return send_request("DESTROY");
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    init_mq();
    char request[MAX_MSG_SIZE];
    snprintf(request, sizeof(request), "SET %d %s %d", key, value1, N_value2);
    return send_request(request);
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    init_mq();
    char request[MAX_MSG_SIZE];
    snprintf(request, sizeof(request), "GET %d", key);
    return send_request(request);
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    init_mq();
    char request[MAX_MSG_SIZE];
    snprintf(request, sizeof(request), "MODIFY %d %s %d", key, value1, N_value2);
    return send_request(request);
}

int delete_key(int key) {
    init_mq();
    char request[MAX_MSG_SIZE];
    snprintf(request, sizeof(request), "DELETE %d", key);
    return send_request(request);
}

int exist(int key) {
    init_mq();
    char request[MAX_MSG_SIZE];
    snprintf(request, sizeof(request), "EXIST %d", key);
    return send_request(request);
}
