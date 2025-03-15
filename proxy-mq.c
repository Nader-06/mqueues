#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include "claves.h"

#define SERVER_QUEUE "/server_queue"
#define CLIENT_QUEUE_TEMPLATE "/client_queue_%d"

// Message structure
typedef struct {
    int request_type;
    int key;
    char value1[256];
    int N_value2;
    double V_value2[32];
    struct Coord value3;
    char client_queue_name[64];
} Message;

int send_request(Message *msg, int *response) {

    // Create unique client queue name
    char client_queue_name[64];
    snprintf(client_queue_name, sizeof(client_queue_name), CLIENT_QUEUE_TEMPLATE, getpid());
    strcpy(msg->client_queue_name, client_queue_name);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Message);
    attr.mq_curmsgs = 0;

    // Create client queue
    mqd_t client_queue = mq_open(client_queue_name, O_CREAT | O_RDONLY, 0666, &attr);
    if (client_queue == -1) {
        perror("mq_open (client queue)");
        return -1;
    }

    // Open server queue
    mqd_t server_queue = mq_open(SERVER_QUEUE, O_WRONLY);
    if (server_queue == -1) {
        perror("mq_open (server)");
        mq_unlink(client_queue_name);
        return -1;
    }

    printf("Proxy: Sending request type=%d, key=%d to server\n", msg->request_type, msg->key);
    
    printf("Proxy: Sending Message size = %ld bytes\n", sizeof(Message));

    // Send message to server
    if (mq_send(server_queue, (char *)msg, sizeof(Message), 0) == -1) {
        perror("mq_send");
        mq_close(server_queue);
        mq_unlink(client_queue_name);
        return -1;
    }

    mq_close(server_queue);
    printf("Proxy: Expecting Message size = %ld bytes\n", sizeof(Message));

    printf("Proxy: Waiting for response from server\n");

    // Receive response
    if (msg->request_type == 2) { // get_value needs full Message
        if (mq_receive(client_queue, (char *)msg, sizeof(Message), NULL) == -1) {
            perror("mq_receive");
            mq_close(client_queue);
            mq_unlink(client_queue_name);
            return -1;
        }
        printf("Proxy: Received response for get_value: value1=%s, N_value2=%d, value3=(%d, %d)\n", msg->value1, msg->N_value2, msg->value3.x, msg->value3.y);
    } else { // All other requests receive just an int
        if (mq_receive(client_queue, (char *)response, sizeof(int), NULL) == -1) {
            perror("mq_receive");
            mq_close(client_queue);
            mq_unlink(client_queue_name);
            return -1;
        }
        printf("Proxy: Received response %d\n", *response);
    }

    mq_close(client_queue);
    mq_unlink(client_queue_name);
    return 0;
}

// API Functions
int destroy() {
    Message msg = { .request_type = 6 };
    int response;
    return send_request(&msg, &response) == 0 ? response : -1;
}

int set_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    Message msg = { .request_type = 1, .key = key, .N_value2 = N_value2, .value3 = value3 };
    strncpy(msg.value1, value1, 255);
    msg.value1[255] = '\0';
    memcpy(msg.V_value2, V_value2, N_value2 * sizeof(double));
    int response;
    return send_request(&msg, &response) == 0 ? response : -1;
}

int get_value(int key, char *value1, int *N_value2, double *V_value2, struct Coord *value3) {
    Message msg = { .request_type = 2, .key = key };
    if (send_request(&msg, NULL) != 0) return -1;
    strncpy(value1, msg.value1, 255);
    value1[255] = '\0';
    *N_value2 = msg.N_value2;
    memcpy(V_value2, msg.V_value2, (*N_value2) * sizeof(double));
    *value3 = msg.value3;
    return 0;
}

int modify_value(int key, char *value1, int N_value2, double *V_value2, struct Coord value3) {
    Message msg = { .request_type = 3, .key = key, .N_value2 = N_value2, .value3 = value3 };
    strncpy(msg.value1, value1, 255);
    msg.value1[255] = '\0';
    memcpy(msg.V_value2, V_value2, N_value2 * sizeof(double));
    int response;
    return send_request(&msg, &response) == 0 ? response : -1;
}

int delete_key(int key) {
    Message msg = { .request_type = 4, .key = key };
    int response;
    return send_request(&msg, &response) == 0 ? response : -1;
}

int exist(int key) {
    Message msg = { .request_type = 5, .key = key };
    int response;
    return send_request(&msg, &response) == 0 ? response : -1;
}
