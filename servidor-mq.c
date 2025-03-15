#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <unistd.h>
#include "claves.h"

#define SERVER_QUEUE "/server_queue"
#define CLIENT_QUEUE_PREFIX "/client_queue_"
#define MAX_MSG_SIZE 1024
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

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

void process_request(Message *msg) {
    int result;
    printf("Received request type: %d for key: %d from client: %s\n", msg->request_type, msg->key, msg->client_queue_name);

    switch (msg->request_type) {
        case 1: // set_value
            printf("Processing set_value: key=%d, value1=%s, N_value2=%d, value3=(%d, %d)\n", msg->key, msg->value1, msg->N_value2, msg->value3.x, msg->value3.y);
            result = set_value(msg->key, msg->value1, msg->N_value2, msg->V_value2, msg->value3);
            break;
        case 2: // get_value
            printf("Processing get_value: key=%d\n", msg->key);
            result = get_value(msg->key, msg->value1, &msg->N_value2, msg->V_value2, &msg->value3);
            printf("Retrieved: value1=%s, N_value2=%d, value3=(%d, %d)\n", msg->value1, msg->N_value2, msg->value3.x, msg->value3.y);
            break;
        case 3: // modify_value
            printf("Processing modify_value: key=%d, new_value1=%s, N_value2=%d, new_value3=(%d, %d)\n", msg->key, msg->value1, msg->N_value2, msg->value3.x, msg->value3.y);
            result = modify_value(msg->key, msg->value1, msg->N_value2, msg->V_value2, msg->value3);
            break;
        case 4: // delete_key
            printf("Processing delete_key: key=%d\n", msg->key);
            result = delete_key(msg->key);
            break;
        case 5: // exist
            printf("Processing exist: key=%d\n", msg->key);
            result = exist(msg->key);
            break;
        case 6: // destroy
            printf("Processing destroy\n");
            result = destroy();
            break;
        default:
            printf("Invalid request type: %d\n", msg->request_type);
            result = -1;
    }

    printf("Response for key %d: %d\n", msg->key, result);

    // Send response back to client
    mqd_t client_queue = mq_open(msg->client_queue_name, O_WRONLY);
    if (client_queue == -1) {
        perror("mq_open (client queue)");
        return;
    }
    mq_send(client_queue, (char *)&result, sizeof(int), 0);
    mq_close(client_queue);
}

int main() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(Message);
    attr.mq_curmsgs = 0;
    
    // Create message queue
    mqd_t server_queue = mq_open(SERVER_QUEUE, O_CREAT | O_RDONLY, 0666, &attr);
    if (server_queue == -1) {
        perror("mq_open (server)");
        exit(1);
    }
    
    printf("Server started, waiting for requests...\n");
    
    while (1) {
        Message msg;
        if (mq_receive(server_queue, (char *)&msg, sizeof(Message), NULL) == -1) {
            perror("mq_receive");
            continue;
        }
        process_request(&msg);
    }
    
    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE);
    return 0;
}