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


    //Insertion at index
    insert_at(list, 3, 20);
    insert_at(list, 4, 21);

    printListTest(list,"Insertion at index test:");

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

    // Deleting at index 
    delete_at(list, 1);

    printListTest(list,"Deleting nodes at index test:");


    // Getting nodes at index
    Node* testNode = get_node_at(list,0);
    printf("Getting node at index:\nget_node_at(list,0).data = %d\n\n", testNode->data);

    // Getting index of node
    size_t testIndex = get_index_of(list, testNode);
    printf("Getting index of the node:\nget_index_of(list,testNode) = %ld\n\n", testIndex);

    // Finding node with given data
    printf("Finding a node with given data:\nget_index_of(list,find_node(list,0)) = %ld\n\n", get_index_of(list,find_node(list,0)));
    
    // Merge test
    List *listMerge = create_list();
    for(int i = 0;i<5;i++){
        push_back(listMerge,i);
    }

    printListTest(list,"Current list visualisation:");
    printListTest(listMerge,"List to merge visualisation:");

    merge(list,listMerge);

    printListTest(list,"List after merge:");

    // Split test

    List *listSplit = split_at(list,5);
    printListTest(list,"1 List after split:");
    printListTest(listSplit,"2 List after split:");

    // Empty test
    printf("is the 1 List empty?: %d\n\n", empty(list));

    // Freeing list
    free_list(list);
    free_list(listSplit);
}