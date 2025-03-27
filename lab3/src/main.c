#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "../include/list.h"

#define LISTS_ARRAY_MAX_SIZE 32
#define FILENAME "save.dat"


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


void saveListToFile(const char *filename, List* list) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Cannot open file for writing");
        exit(1);
    }
    
    // Zapisujemy rozmiar listy
    fwrite(&list->size, sizeof(size_t), 1, file);
    
    // Iterujemy przez listę i zapisujemy węzły
    Node *current = list->frontNode;
    while (current) {
        fwrite(&current->data, sizeof(int), 1, file);
        current = current->nextNode;
    }
    
    fclose(file);
}


void loadListFromFile(const char *filename, List* list) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Cannot open file for reading");
        exit(1);
    }
    
    // Czyścimy istniejącą listę, jeśli ma jakieś dane
    free_list_nodes(list);
    list->frontNode = NULL;
    list->endNode = NULL;
    list->size = 0;
    
    // Wczytujemy rozmiar listy
    size_t size;
    fread(&size, sizeof(size_t), 1, file);
    
    // Wczytujemy wartości i tworzymy nowe węzły
    for (size_t i = 0; i < size; i++) {
        int value;
        fread(&value, sizeof(int), 1, file);
        push_back(list, value);
    }
    
    fclose(file);
}


void initSaveFile(const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (file) fclose(file);
}

void startForking(){
    List *list = create_list(); 
    pid_t pid;

    loadListFromFile(FILENAME, list); 

    while(list->size < 10){
        pid = fork(); 

        if (pid == -1) {
            perror("Cant create new process!");
            exit(1);
        }
        else if (pid == 0) { // Proces potomny
            printf("Proces potomny %d dodaje liczbe %ld do listy\n", getpid(), list->size);
            printListTest(list,"Lista tego procesu");
            push_back(list, list->size);
            saveListToFile(FILENAME, list);
            sleep(1);
        }
        else{
            waitpid(pid, NULL, 0);
            loadListFromFile(FILENAME,list);
        }
    }

    scanf("");
    
}



int main(int argc, char **argv){
    List *list = create_list();

    initSaveFile(FILENAME);

    push_back(list,0);
    push_back(list,1);
    push_back(list,2);

    
    saveListToFile(FILENAME,list);
    
    startForking();

}