#ifndef LIST_H
#define LIST_H

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
} List;

/**
 * Struct with process' informations, carried inside ListItem.
 * @param pid Process id number.
 * @param priority Priority in scheduler, the lower the faster.
 * @param params Process params vector.
 */
struct Item {
  int pid;
  int priority;
  char** params;
};

/**
 * Creates an empty dynamic double linked list of processes.
 * @return Empty dynamic double linked list of processes.
 */
List* createList();

/**
 * Creates a struct with all of a processes information.
 * @param pid Process id number.
 * @param priority Priority in scheduler, the lower the faster.
 * @param params Process params vector.
 * @return Struct containing info of a process.
 */
Item* createItem(int, int, char**);

/**
 * Gets a process struct in the position.
 * @param list List to get item from.
 * @param position Position of item wished to be taken.
 * @return Item in the list's position. NULL if fail.
 */
Item* getItem(List*, unsigned);

/**
 * Puts a process struct into a ListItem struct and pushes into a list's front.
 * @param list List to push item to.
 * @param item Item to be pushed into list.
 */
void pushFront(List*, Item*);

/**
 * Puts a process struct into a ListItem struct and pushes into a list's back.
 * @param list List to push item to.
 * @param item Item to be pushed into list.
 */
void pushBack(List*, Item*);

/**
 * Puts a process struct into a ListItem struct, pushes into a list's position.
 * @param list List to push item to.
 * @param position Position wished to insert item into.
 * @param item Item to be pushed into list.
 * @return Zero if success, failure otherwise.
 */
int insertItem(List*, unsigned, Item*);

/**
 * Takes first item from the list.
 * @param list List to be popped.
 * @return Popped item.
 */
Item* popFront(List*);

/**
 * Takes last item from the list.
 * @param list List to be popped.
 * @return Popped item.
 */
Item* popBack(List*);

/**
 * Takes item in position from the list.
 * @param list List to be popped.
 * @param position Position of item to be popped.
 * @return Popped item.
 */
Item* popItem(List*, unsigned);

/**
 * Frees all memory allocation in list - items, itemlists, and the list itself.
 * @param list List to have its mallocs freed.
 */
void freeList(List*);

/**
 * Prints all the processes' pids from first to last.
 * @param list List to have all its pids printed.
 */
void printAll(List*);

#endif  // LIST_H
