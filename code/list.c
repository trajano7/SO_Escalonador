#include "list.h"

#include <stdio.h>
#include <stdlib.h>

ProcList* createList() {
  ProcList* list = (ProcList*)malloc(sizeof(ProcList));
  list->lenght = 0;
  list->first = NULL;
  list->last = NULL;
  return list;
}

void pushFront(ProcList* list, Item* item) {
  ListItem* aux = (ListItem*)malloc(sizeof(ListItem));
  aux->item = item;
  item->parent = aux;
  aux->right = aux->left = NULL;
  if (list->lenght == 0) {
    list->last = aux;
  } else {
    aux->right = list->first;
    list->first->left = aux;
  }
  list->first = aux;
  list->lenght++;
}

void pushBack(ProcList* list, Item* item) {
  ListItem* aux;
  if (list->lenght == 0) {
    pushFront(list, item);
    return;
  }
  aux = (ListItem*)malloc(sizeof(ListItem));
  aux->item = item;
  item->parent = aux;
  aux->right = NULL;
  aux->left = list->last;
  list->last->right = aux;
  list->last = aux;
  list->lenght++;
}

int insertItem(ProcList* list, unsigned position, Item* item) {
  ListItem* iterator;
  ListItem* aux;
  if (position > list->lenght) {
    return -1;
  }
  if (position == 0) {
    pushFront(list, item);
    return 0;
  }
  if (position == list->lenght) {
    pushBack(list, item);
    return 0;
  }
  iterator = list->first;
  for (unsigned i = 0; i < position; i++) {
    iterator = iterator->right;
  }
  aux = (ListItem*)malloc(sizeof(ListItem));
  aux->item = item;
  item->parent = aux;
  aux->left = iterator->left;
  aux->right = iterator;
  iterator->left->right = aux;
  iterator->left = aux;
  list->lenght++;
  return 0;
}

Item* getItem(ProcList* list, unsigned position) {
  ListItem* aux;
  if (position >= list->lenght) {
    return NULL;
  }
  aux = list->first;
  for (unsigned i = 0; i < position; i++) {
    aux = aux->right;
  }
  return aux->item;
}

Item* popFront(ProcList* list) {
  ListItem* aux;
  Item* item;
  if (list->lenght == 0) {
    return NULL;
  }
  if (list->lenght == 1) {
    item = list->first->item;
    free(list->first);
    list->first = list->last = NULL;
    list->lenght--;
    return item;
  }
  aux = list->first;
  aux->right->left = NULL;
  list->first = aux->right;
  item = aux->item;
  item->parent = NULL;
  free(aux);
  list->lenght--;
  return item;
}

Item* popBack(ProcList* list) {
  ListItem* aux;
  Item* item;
  if (list->lenght == 0) {
    return NULL;
  }
  if (list->lenght == 1) {
    item = list->first->item;
    free(list->first);
    list->first = list->last = NULL;
    list->lenght--;
    return item;
  }
  aux = list->last;
  aux->left->right = NULL;
  list->last = aux->left;
  item = aux->item;
  item->parent = NULL;
  free(aux);
  list->lenght--;
  return item;
}

int deleteItem(ProcList* list, Item* item) {
  if (item == NULL) {
    return -1;
  }
  if (item->parent == NULL) {
    return -1;
  }
  if (list->lenght == 1) {
    list->first = list->last = NULL;
    list->lenght--;
    free(item->parent);
    free(item);
    return 0;
  }
  if (item->parent->left == NULL) {
    item->parent->right->left = NULL;
    list->first = item->parent->right;
  } else if (item->parent->right == NULL) {
    item->parent->left->right = NULL;
    list->last = item->parent->left;
  } else {
    item->parent->left->right = item->parent->right;
    item->parent->right->left = item->parent->left;
  }
  list->lenght--;
  free(item->parent);
  free(item);
  return 0;
}

void freeList(ProcList* list) {
  while (list->lenght > 0) {
    free(popBack(list));
  }
  free(list);
}

Item* popItem(ProcList* list, unsigned position) {
  ListItem* iterator;
  Item* item;
  if (position >= list->lenght) {
    return NULL;
  }
  if (position == 0) {
    return popFront(list);
  }
  if (position == list->lenght - 1) {
    return popBack(list);
  }
  iterator = list->first;
  for (unsigned i = 0; i < position; i++) {
    iterator = iterator->right;
  }
  item = iterator->item;
  iterator->left->right = iterator->right;
  iterator->right->left = iterator->left;
  item->parent = NULL;
  free(iterator);
  list->lenght--;
  return item;
}

Item* createItem(int pid, int priority, char* programName, char** params) {
  Item* item = (Item*)malloc(sizeof(Item));
  item->parent = NULL;
  item->pidVirtual = pid;
  item->programName = programName;
  item->priority = priority;
  item->params = params;
  return item;
}

void printAll(ProcList* list) {
  ListItem* aux = list->first;
  printf("printAll:\n");
  for (unsigned i = 0; i < list->lenght; i++) {
    printf("pid %d = %d\n", i, aux->item->pidVirtual);
    aux = aux->right;
  }
}

Item* findItem(ProcList* list, int pid) {
  ListItem* iterator = list->first;
  for (unsigned i = 0; i < list->lenght; i++) {
    if (iterator->item->pidVirtual == pid) {
      return iterator->item;
    }
    iterator = iterator->right;
  }
  return NULL;
}

// All working!
// cc list.c -o list -Wall -Wextra -pedantic -g -O0
// cppcheck list.c list.h
// valgrind --leak-check=yes -s ./list
/*
int main() {
  ProcList* myList = createList();
  Item* aux;
  Item* item1 = createItem(1, 10, NULL);
  Item* item2 = createItem(2, 110, NULL);
  Item* item3 = createItem(3, 310, NULL);
  Item* item4 = createItem(4, 150, NULL);
  Item* item5 = createItem(5, 810, NULL);
  Item* item6 = createItem(6, 109, NULL);
  Item* item7 = createItem(7, 106, NULL);
  Item* item8 = createItem(8, 150, NULL);
  Item* item9 = createItem(9, 110, NULL);
  Item* item10 = createItem(10, 120, NULL);
  Item* item11 = createItem(11, 1520, NULL);
  Item* item12 = createItem(12, 1620, NULL);
  pushBack(myList, item1);
  pushBack(myList, item2);
  pushBack(myList, item3);
  pushBack(myList, item4);
  pushBack(myList, item5);
  pushBack(myList, item6);
  pushBack(myList, item7);

  printAll(myList);

  printf("\nInit\n");
  printf("first = %d\n", myList->first->item->pidVirtual);
  printf("last = %d\n", myList->last->item->pidVirtual);

  if ((aux = popFront(myList)) != NULL) {
    printf("\npopFront\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
    printf("aux = %d\n", aux->pidVirtual);
  } else {
    printf("\npopFront failed!\n");
  }
  free(aux);

  if ((aux = popBack(myList)) != NULL) {
    printf("\npopBack\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
    printf("aux = %d\n", aux->pidVirtual);
  } else {
    printf("\npopBack failed!\n");
  }
  free(aux);

  pushFront(myList, item8);
  printf("\npushFront\n");
  printf("first = %d\n", myList->first->item->pidVirtual);
  printf("last = %d\n", myList->last->item->pidVirtual);

  pushBack(myList, item9);
  printf("\npushBack\n");
  printf("first = %d\n", myList->first->item->pidVirtual);
  printf("last = %d\n", myList->last->item->pidVirtual);

  if (insertItem(myList, 2, item10) == 0) {
    printf("\ninsertItem (pos = 2)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
  } else {
    printf("\ninsertItem (pos = 2) failed!\n");
  }

  if (insertItem(myList, 0, item11) == 0) {
    printf("\ninsertItem (pos = 0)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
  } else {
    printf("\ninsertItem (pos = 0) failed!\n");
  }

  if (insertItem(myList, myList->lenght, item12) == 0) {
    printf("\ninsertItem (pos = myList->length)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
  } else {
    printf("\ninsertItem (pos = myList->length) failed!\n");
  }

  printf("\n");
  printAll(myList);

  if ((aux = getItem(myList, 2)) != NULL) {
    printf("\ngetItem (pos = 2)\n");
    printf("aux = %d\n", aux->pidVirtual);
  } else {
    printf("getItem (pos = 2) failed!\n");
  }

  if ((aux = popItem(myList, 2)) != NULL) {
    printf("\npopItem (pos = 2)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
    printf("aux = %d\n", aux->pidVirtual);
  } else {
    printf("\npopItem (pos = 2) failed!\n");
  }
  free(aux);

  if ((aux = popItem(myList, 0)) != NULL) {
    printf("\npopItem (pos = 0)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
    printf("aux = %d\n", aux->pidVirtual);
  } else {
    printf("\npopItem (pos = 0) failed!\n");
  }
  free(aux);

  if ((aux = popItem(myList, myList->lenght - 1)) != NULL) {
    printf("\npopItem (pos = myList->lenght - 1)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
    printf("aux = %d\n", aux->pidVirtualVirtual);
  } else {
    printf("\npopItem (pos = myList->lenght - 1) failed!\n");
  }
  free(aux);

  if ((aux = findItem(myList, 5)) != NULL) {
    printf("\nfindItem (pid = 5)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
    printf("pid = %d, priority = %d\n", aux->pidVirtualVirtual, aux->priority);
  } else {
    printf("\nfindItem (pid = 5) failed!\n");
  }

  if (deleteItem(myList, aux) == 0) {
    printf("\ndeleteItem (pid = 5)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
  } else {
    printf("\ndeleteItem (pid = 5) failed!\n");
  }

  if ((aux = getItem(myList, 0)) != NULL) {
    printf("\ngetItem (pos = 0)\n");
    printf("aux = %d\n", aux->pidVirtualVirtual);
  } else {
    printf("getItem (pos = 0) failed!\n");
  }

  if (deleteItem(myList, aux) == 0) {
    printf("\ndeleteItem (pos = 0)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
  } else {
    printf("\ndeleteItem (pos = 0) failed!\n");
  }

  if ((aux = getItem(myList, myList->lenght - 1)) != NULL) {
    printf("\ngetItem (pos = myList->lenght - 1)\n");
    printf("aux = %d\n", aux->pidVirtualVirtual);
  } else {
    printf("getItem (pos = myList->lenght - 1) failed!\n");
  }

  if (deleteItem(myList, aux) == 0) {
    printf("\ndeleteItem (pos = myList->lenght - 1)\n");
    printf("first = %d\n", myList->first->item->pidVirtual);
    printf("last = %d\n", myList->last->item->pidVirtual);
  } else {
    printf("\ndeleteItem (pos = myList->lenght - 1) failed!\n");
  }

  printf("\n");
  printAll(myList);
  printf("list length = %d\n", myList->lenght);

  freeList(myList);
  return 0;
}
*/
