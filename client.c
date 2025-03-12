#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<mqueue.h>
#include<sys/stat.h>
#include "claves.h"

#define MAX_MSG_SIZE 1024
#define MQ_NAME "/mq_servidor"

int main(int argc, char *argv[]){
    mqd_t mq;
    struct mq_attr attr;
    char buffer[MAX_MSG_SIZE];
    
    //attributes
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    //open the message queue
    mq = mq_open(MQ_NAME, O_WRONLY);
    if(mq == (mqd_t)-1){ //the error on queue_open
        perror("mq_open");
        exit(1);
    }

    printf("Cliente de mensajes iniciado\n");
    
    memset(buffer, 0, MAX_MSG_SIZE);//clear the buffer for memory safety


    char input[MAX_MSG_SIZE] ;
    printf("Ingrese un mensaje: ");
    fgets(input, MAX_MSG_SIZE, stdin);
    strncpy(buffer, input, strlen(input)-1);

    
    
    ssize_t bytes_sent = mq_send(mq, buffer, strlen(buffer)+1, 0);
    if(bytes_sent == -1){
        perror("mq_send");
        exit(1);
    }

    //close the message queue
    mq_close(mq);

    return 0;
}