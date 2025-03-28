#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdbool.h>

#define LIST_INDEX_NOT_FOUND ((size_t)-1)

// Node used for List
typedef struct Node Node;
struct Node{
    Node* nextNode;
    Node* prevNode;
    int data;
};

// Double Linked List 
typedef struct List List;
struct List{
    Node* frontNode;
    Node* endNode;
    size_t size;
};

/* INITIALIZATION FUNCTIONS */

// Prepares list data structure to be used 
// or allocates its memory if its a NULL pointer.
//
// Returns initialized list or NULL if it couldnt allocate memory.
List* initialize_list(List* list);

// Creates new instance of List, allocates new memory
//
// Returns allocated and initialized instance of List
List* create_list();

/* INSERTION FUNCTIONS */

// Pushes new node at the back of the list (after endNode).
//
// Returns a inserted node or NULL if it couldnt allocate memory.
Node* push_back(List *list,int data);

// Pushes new node at the front of the list (before startNode)
//
// Returns a inserted node or NULL if it couldnt allocate memory.
Node* push_front(List *list, int data);

// Inserts new node before argument node
//
// Returns a inserted node or NULL if it couldnt allocate memory.
Node* insert_before(List *list, Node *node, int data);

// Inserts new node after argument node
//
// Returns a inserted node or NULL if it couldnt allocate memory.
Node* insert_after(List *list, Node *node, int data);

// Inserts new node at given index counted from the start node.
//
// Returns a inserted node or NULL if it couldnt allocate memory.
Node* insert_at(List *list, size_t index, int data);

/* DELETION FUNCTIONS */

// Pops node at the back of the list (pops endNode).
void pop_back(List *list);

//Pops node at the front of the list (pops startNode)
void pop_front(List *list);

// Deletes given node
void delete_node(List *list, Node *node);

// Deletes node at given index counted from the start node.
void delete_at(List *list, size_t index);

/* ACCESS FUNCTIONS */

// Returns a node at given index
Node* get_node_at(List *list, size_t index);

// Returns a index of a given node.
// If it couldnt find it, returns MAX_SIZE of size_t.
size_t get_index_of(List *list, Node* node);

// Finds and returns the first node containing the given data.
// Returns NULL if not found.
Node* find_node(List* list, int data);

/* MERGE AND SPLIT FUNCTIONS */

// Merges List rhs to List lhs. 
// If rhs is empty, lhs remains unchanged.
// WARNING Frees rhs list regardless of its state.
//
// Returns a pointer to the merged lhs list.
List* merge(List* lhs, List* rhs);

// Splits list at given index and creates new list.
// Creates a new list instance.
//
// Returns new list instance or NULL if it couldnt allocate memory.
List* split_at(List* list, size_t index);

/* STATUS FUNCTIONS */

// Returns true if size is equal to 0, otherwise returns false.
bool is_empty(List *list);

/* DEALLOCATION FUNCTIONS */

// Frees list nodes.
//
// To also free list use free_list().
void free_list_nodes(List *list);

// Frees list given as a argument,
// frees both Nodes and List itself.
//
// To only free nodes use free_list_nodes().
void free_list(List *list);

#endif