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
    switch (msg->request_type) {
        case 1: // set_value
            result = set_value(msg->key, msg->value1, msg->N_value2, msg->V_value2, msg->value3);
            break;
        case 2: // get_value
            result = get_value(msg->key, msg->value1, &msg->N_value2, msg->V_value2, &msg->value3);
            break;
        case 3: // modify_value
            result = modify_value(msg->key, msg->value1, msg->N_value2, msg->V_value2, msg->value3);
            break;
        case 4: // delete_key
            result = delete_key(msg->key);
            break;
        case 5: // exist
            result = exist(msg->key);
            break;
        case 6: // destroy
            result = destroy();
            break;
        default:
            result = -1;
    }
    
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
