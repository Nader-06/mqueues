#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "claves.h"

typedef struct key_values{
    int key;
    char value_1[256];
    double value_2[32];
    int N_value_2;
    struct Coord value_3;
    struct key_values *next;//key value will be the pointer to our nodes in the linked list
}key_value;

key_value *head = NULL;
int destroy(){
    key_value *current = head;
    while(current!=NULL){
        key_value *temp = current;
        current = current->next;
        free(temp);
    }
    head = NULL;
    return 0;
}

int set_value(int key,char *value_1,int N_value_2,double *value_2,struct Coord  value_3){
    if(key<0 || strlen(value_1)>255 || N_value_2<1 || N_value_2>32){ //if N is out of  range or key less than 0
        return -1;
    }
    key_value *current = head;
    while(current!=NULL){
        if(current->key == key){//make sure no repeated keys
            fprintf(stderr,"Key already exists\n");
            return -1;
        }
        current = current->next;

    }
    key_value *new_node = (key_value*)malloc(sizeof(key_value));
    
    if(!new_node){
        return -1;
    }
    //store key values
    new_node->key = key;

    strncpy(new_node->value_1,value_1,255);
    new_node->value_1[255] = '\0';//null termination

    new_node->N_value_2 = N_value_2;

    for (int i = 0; i<N_value_2; i++){
        new_node->value_2[i]= value_2[i];
    }

    new_node->value_3 = value_3;

    new_node->next = head;//link new node
    head = new_node;
    return 0;
}

int get_value(int key,char *value_1,int *N_value_2,double *V_value_2,struct Coord *value_3){
    if(key<0||value_1 == NULL||N_value_2==NULL||V_value_2==NULL){
        fprintf(stderr,"Invalid parameters\n");
        return -1;
    }
    key_value *current = head;
    while(current!=NULL&&current->key!=key){
        current = current->next;
    }
    if(current==NULL){
        fprintf(stderr,"Key not found\n");
        return -1;
    }
    //get values 
    strncpy(value_1,current->value_1,255);
    *N_value_2 = current->N_value_2;
    for (int i =0; i <current->N_value_2; i++){
        V_value_2[i] = current->value_2[i];
    }
    *value_3 = current->value_3;
    return 0;
}

int modify_value(int key,char *value_1,int N_value_2,double *V_value_2,struct Coord value_3){
    if(key<0 ||value_1 == NULL || strlen(value_1)>255 || N_value_2<1 || N_value_2>32){ 
        fprintf(stderr,"Invalid parameters\n");
        return -1;
    }
    key_value *current = head;
    while(current!=NULL&&current->key!=key){
        current = current->next;
    }
    if(current==NULL){
        fprintf(stderr,"Key not found\n");
        return -1;
    }    
    //modify values
    strncpy(current->value_1,value_1,255);
    current->value_1[255] = '\0';
    current->N_value_2 = N_value_2;
    for (int i =0; i <current->N_value_2; i++){
        current->value_2[i] = V_value_2[i];
    }
    current->value_3 = value_3;
    fprintf(stderr,"key: %d, value1: %s, N_value2: %d, value2: %f %f %f, value3: %d %d\n",current->key,current->value_1,current->N_value_2,current->value_2[0],current->value_2[1],current->value_2[2],current->value_3.x,current->value_3.y);
    return 0;
    
}

int delete_key(int key){
    if(key<0){
        fprintf(stderr,"Invalid key\n");
        return -1;
    }
    key_value *current = head;
    key_value *prev = NULL;
    if(current == NULL){
        fprintf(stderr,"List is empty\n");
        return -1;
    }
    while(current!=NULL && current->key != key){
        prev = current;
        current = current->next;

    }
    if(current==NULL){
        fprintf(stderr,"Key not found\n");
        return -1;
    }
    else{
        if(prev == NULL){//deleting the head
            head = current->next;
            free(current);//remove from memory space
        }
        else{
            prev->next = current->next;
            free(current);
        }
    }       
    return 0;
}

int exist(int key){
    if(key<0){
        fprintf(stderr,"Invalid key\n");
        return 0;//in this case 0 for key not found
    }
    key_value *current = head;
    while(current!=NULL){
        if(current->key == key){
            fprintf(stderr,"Key found\n");
            return 1;//key found-> node found
        }
        current = current->next;
    }
    fprintf(stderr,"Key not found\n");
    return 0;//key not found
}
