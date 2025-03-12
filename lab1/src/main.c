#include <stdio.h>
#include "../include/list.h"


void printListTest(List* list, const char* testTitle){
    Node  *currentNode = list->frontNode;
    printf("%s\n",testTitle);
    printf("List size: %ld\n", list->size);
    
    while(currentNode != NULL){
        printf("<- %d ->", currentNode->data);
        currentNode = currentNode->nextNode;
    }
    printf("\n\n");
}

int main(){
    printf  ("****************************\n*     List testing stuff   *\n****************************\n\n\n");



    List* list = create_list();

    //Push back testing
    for(int i = 0;i<3;i++){
        push_back(list,i);
    }

    printListTest(list,"Push back test:");

    //Push front testing
    for(int i = 0;i<3;i++){
        push_front(list,i);
    }

    printListTest(list,"Push front test:");

    //Insertions at both ends testing
    insert_after(list, list->endNode,5);
    insert_before(list, list->frontNode,5);

    printListTest(list,"Insertion at ends test:");

    //Insertion in between ends testing
    Node* currentNode = list->frontNode;
    while(currentNode != NULL){
        if(currentNode->data == 0) insert_after(list,currentNode,15);
        currentNode = currentNode->nextNode;
    }

    currentNode = list->frontNode;
    while(currentNode != NULL){
        if(currentNode->data == 0) insert_before(list,currentNode,15);
        currentNode = currentNode->nextNode;
    }

    printListTest(list,"Insertion in between ends test:");


    //Popping back and from testing
    pop_back(list);
    pop_front(list);

    printListTest(list,"Popping test:");

    //Deleting every not even number from list to test deleting nodes
    Node *tempNode = NULL;
    currentNode = list->frontNode;
    while(currentNode != NULL){
        tempNode = currentNode->nextNode;
        if(currentNode->data % 2 == 1){
            delete_node(list,currentNode);
        }
        currentNode = tempNode;
    }

    printListTest(list,"Deleting nodes test:");

    free_list(list);
}