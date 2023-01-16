#ifndef LIST_H
#define LIST_H

#include <time.h>

typedef struct ListItem ListItem;

typedef struct Item Item;

/**
 * Position struct in list. Process' data is carried inside it.
 * @param item Item struct inside this struct.
 * @param right Item to the right of this struct in the list.
 * @param left Item to the left of this struct in the list.
 */
struct ListItem {
  Item* item;
  ListItem* right;
  ListItem* left;
};

/**
 * Dynamic double linked list.
 * @param length Current amount of items in this list.
 * @param first First item in this list.
 * @param last Last item in this list.
 */
typedef struct {
  unsigned lenght;
  ListItem* first;
  ListItem* last;
} ProcList;

/**
 * Struct with process' informations, carried inside ListItem.
 * @param parent ListItem that holds this item.
 * @param pid Process id number.
 * @param priority Priority in scheduler, the lower the faster.
 * @param params Process params vector.
 */
struct Item {
  ListItem* parent;
  char programName[30];
  int pidVirtual;
  int pidReal;
  int priority;
  int quantumTimes;  
  time_t startTime;
  int dynamicCriteria;  
  char** params;
};

/**
 * Creates an empty dynamic double linked list of processes.
 * @return Empty dynamic double linked list of processes.
 */
ProcList* createList();

/**
 * Creates a struct with all of a processes information.
 * @param pid Process id number.
 * @param priority Priority in scheduler, the lower the faster.
 * @param params Process params vector.
 * @return Struct containing info of a process.
 */
Item* createItem(int, int, char programName[30], char**);

/**
 * Gets a process struct in the position.
 * @param list List to get item from.
 * @param position Position of item wished to be taken.
 * @return Item in the list's position. NULL if fail.
 */
Item* getItem(ProcList*, unsigned);

/**
 * Finds the item with specified pid.
 * @param list List to be popped.
 * @param pid Id number of process to be popped.
 * @return Item found with given pid. NULL if fail.
 */
Item* findItem(ProcList*, int);

/**
 * Puts a process struct into a ListItem struct and pushes into a list's front.
 * @param list List to push item to.
 * @param item Item to be pushed into list.
 */
void pushFront(ProcList*, Item*);

/**
 * Puts a process struct into a ListItem struct and pushes into a list's back.
 * @param list List to push item to.
 * @param item Item to be pushed into list.
 */
void pushBack(ProcList*, Item*);

/**
 * Puts a process struct into a ListItem struct, pushes into a list's position.
 * @param list List to push item to.
 * @param position Position wished to insert item into.
 * @param item Item to be pushed into list.
 * @return Zero if success, failure otherwise.
 */
int insertItem(ProcList*, unsigned, Item*);

/**
 * Takes first item from the list.
 * @param list List to be popped.
 * @return Popped item.
 */
Item* popFront(ProcList*);

/**
 * Takes last item from the list.
 * @param list List to be popped.
 * @return Popped item.
 */
Item* popBack(ProcList*);

/**
 * Takes item in position from the list.
 * @param list List to be popped.
 * @param position Position of item to be popped.
 * @return Popped item.
 */
Item* popItem(ProcList*, unsigned);

/**
 * Deletes the item in the list and its ListItem container.
 * @param list List to have item deleted.
 * @param item Item to be deleted.
 * @return Zero if success, failure otherwise.
 */
int deleteItem(ProcList*, Item*);

/**
 * Frees all memory allocation in list - items, itemlists, and the list itself.
 * @param list List to have its mallocs freed.
 */
void freeList(ProcList*);

/**
 * Prints all the processes' pids from first to last.
 * @param list List to have all its pids printed.
 */
void printAll(ProcList*);

#endif  // LIST_H
