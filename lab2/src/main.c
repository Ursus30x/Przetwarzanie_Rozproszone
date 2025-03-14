#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../include/list.h"

#define LISTS_ARRAY_MAX_SIZE 32

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

void showMenu(List *list,int *optionPtr){
    system("clear");
    printf("*******************************\n");
    printf("*       LIST TESTER v1.0      *\n");
    printf("*******************************\n");
    printf("\n");
    printListTest(list,"List status:");
    printf("*******************************\n");
    printf("            MENU               \n");
    printf("1. Push back\n");
    printf("2. Push front\n");
    printf("3. Pop back\n");
    printf("4. Pop front\n");
    printf("Commad: ");
    scanf("%d", optionPtr);
}

void hadndleInput(List *list,int option){
    int data = 0;

    switch(option){
        case 1:
            printf("Enter data: ");
            scanf("%d",&data);
            push_back(list,data);

            break;
        case 2:
            printf("Enter data: ");
            scanf("%d",&data);
            push_front(list,data);
            
            break;
        case 3:
            pop_back(list);
            break;
        case 4:
            pop_front(list);
            break;
        default:
            printf("THERES NO SUCH A OPTION");
            usleep(1000);
            break;    
    }
}


int main(int argc, char **argv){
    List *list = create_list();
    bool running = true;


    while(running){
        int option = 0;
        showMenu(list,&option);
        hadndleInput(list,option);

        

    }
}