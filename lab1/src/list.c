
#include "../include/list.h"
#include <stddef.h>
#include <malloc.h>

// Handles first insertion in to the list
static void firstInsert(List *list,Node* node){
    list->endNode   = node;
    list->frontNode = node;
    list->size++;
}

// Initializes node with given arguments
//
// DOEST NOT ALLOCATE NEW NODE IN MEMORY
static Node* initNode(Node* node, Node* nextNode, Node* prevNode, int data){
    node->nextNode  = nextNode;
    node->prevNode  = prevNode;
    node->data      = data;

    return node;
}

List* initialize_list(List* list){
    list->endNode   = NULL;
    list->frontNode = NULL;
    list->size      = 0;

    return list;
}

List* create_list(){
    List* list;
    list = malloc(sizeof(List));

    if(list == NULL)return NULL;
  
    

    return initialize_list(list);
}

Node* push_back(List *list,int data){
    //Setup of new node
    Node* newNode =  malloc(sizeof(Node));

    if(newNode == NULL)return NULL; // Checks if malloc return a valid pointer

    initNode(newNode, NULL, list->endNode, data);

    // Check if we are inserting in to empty list
    if(list->size == 0){
        firstInsert(list,newNode);
        return newNode;
    }

    // Insert new node at the end
    list->endNode->nextNode = newNode;
    list->endNode           = newNode;
    list->size++;

    return newNode;
}

Node* push_front(List *list, int data){
    //Setup of new node
    Node* newNode =  malloc(sizeof(Node));

    if(newNode == NULL)return NULL; // Checks if malloc return a valid pointer

    initNode(newNode, list->frontNode, NULL, data);

    // Check if we are inserting in to empty list
    if(list->size == 0){
        firstInsert(list,newNode);
        return newNode;
    }

    // Insert new node at the end
    list->frontNode->prevNode = newNode;
    list->frontNode           = newNode;
    list->size++;

    return newNode;
}

Node* insert_before(List *list, Node *targetNode, int data){
    Node *newNode = malloc(sizeof(Node));

    if(newNode == NULL)return NULL;

    //Pointing intial previous target node to new node
    if(targetNode->prevNode == NULL)list->frontNode = newNode;
    else targetNode->prevNode->nextNode = newNode;

    //Initializing new node
    initNode(newNode, targetNode, targetNode->prevNode, data);

    //Pointing target node to new node
    targetNode->prevNode = newNode;

    list->size++;

    return newNode;
}

Node* insert_after(List *list, Node *targetNode, int data){
    Node *newNode = malloc(sizeof(Node));

    if(newNode == NULL)return NULL;

    //Pointing intial previous target node to new node
    if(targetNode->nextNode == NULL) list->endNode = newNode;
    else targetNode->nextNode->prevNode = newNode;

    //Initializing new node
    initNode(newNode, targetNode->nextNode, targetNode, data);

    //Pointing target node to new node
    targetNode->nextNode = newNode;

    list->size++;

    return newNode;
}

Node* insert_at(List* list, size_t index, int data){
    Node* targetNode = get_node_at(list, index-1);

    if(targetNode == NULL) return NULL;

    return insert_after(list,targetNode,data);
}

void pop_back(List *list){
    Node* poppedNode = list->endNode;

    if (list->size == 1) {
        list->endNode = list->frontNode = NULL;
    } else {
        list->endNode = list->endNode->prevNode;
        list->endNode->nextNode = NULL;
    }
    list->size--;

    free(poppedNode);
}

void pop_front(List *list){
    Node* poppedNode = list->frontNode;

    if (list->size == 1) {
        list->frontNode = list->endNode = NULL;
    } else {
        list->frontNode = list->frontNode->nextNode;
        list->frontNode->prevNode = NULL;
    }
    list->size--;

    free(poppedNode);
}

void delete_node(List *list, Node *node){
    Node    *nextNode = node->nextNode,
            *prevNode = node->prevNode;

    if (prevNode) prevNode->nextNode = nextNode;
    else list->frontNode = nextNode;
        
    if (nextNode) nextNode->prevNode = prevNode;
    else list->endNode = prevNode;

    list->size--;

    free(node);
}

void delete_at(List *list, size_t index){
    Node* targetNode = get_node_at(list, index);

    if(targetNode == NULL) return;

    delete_node(list, targetNode);
}

Node* get_node_at(List *list, size_t index){
    Node* targetNode = list->frontNode;
    size_t i = 0;

    while(targetNode != NULL && i != index){
        targetNode = targetNode->nextNode;
        i++;
    }

    return targetNode;
}

size_t get_index_of(List *list, Node *node){
    Node* iterNode = list->frontNode;
    size_t i = 0;

    while(iterNode != NULL){
        if(iterNode == node)return i;
        iterNode = iterNode->nextNode;
        i++;
    }

    return -1;
}

Node* find_node(List *list,int data){
    Node* iterNode = list->frontNode;

    while(iterNode != NULL){
        if(iterNode->data == data) return iterNode;
        iterNode = iterNode->nextNode;
    }

    return NULL;
}

List* merge(List *lhs, List *rhs){
    if(rhs == NULL)return lhs;

    lhs->size += rhs->size;

    lhs->endNode->nextNode = rhs->frontNode;
    rhs->frontNode->prevNode = lhs->endNode;
    lhs->endNode = rhs->endNode;

    free(rhs);

    return lhs;
}

List *split_at(List *list, size_t index){
    List *newList = create_list();
    Node *iterNode = get_node_at(list,index);
    size_t rightListSize = list->size - index;

    if(iterNode == NULL)return newList;

    newList->endNode = list->endNode;

    list->endNode = iterNode->prevNode;
    list->endNode->nextNode = NULL;

    newList->frontNode = iterNode;
    newList->frontNode->prevNode = NULL;

    newList->size = rightListSize;
    list->size = list->size-rightListSize;

    return newList;
}

bool empty(List *list){
    return (list->size == 0);
}

void free_list_nodes(List *list){
    Node    *currentNode    = list->frontNode,
            *tempNode       = NULL;

    while(currentNode != NULL){
        tempNode = currentNode->nextNode;
        free(currentNode);
        currentNode = tempNode;
    }
}

void free_list(List *list){
    free_list_nodes(list);
    free(list);
}