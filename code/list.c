#include "list.h"

#include <stdio.h>
#include <stdlib.h>

List* createList() {
  List* list = (List*)malloc(sizeof(List));
  list->lenght = 0;
  list->first = NULL;
  list->last = NULL;
  return list;
}

void pushFront(List* list, Item* item) {
  ListItem* aux = (ListItem*)malloc(sizeof(ListItem));
  aux->item = item;
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

void pushBack(List* list, Item* item) {
  ListItem* aux;
  if (list->lenght == 0) {
    pushFront(list, item);
    return;
  }
  aux = (ListItem*)malloc(sizeof(ListItem));
  aux->item = item;
  aux->right = NULL;
  aux->left = list->last;
  list->last->right = aux;
  list->last = aux;
  list->lenght++;
}

int insertItem(List* list, unsigned position, Item* item) {
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
  aux->left = iterator->left;
  aux->right = iterator;
  iterator->left->right = aux;
  iterator->left = aux;
  list->lenght++;
  return 0;
}

Item* getItem(List* list, unsigned position) {
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

Item* popFront(List* list) {
  ListItem* aux;
  Item* item;
  if (list->lenght == 1) {
    item = list->first->item;
    free(list->first);
    list->first = list->last = NULL;
    return NULL;
  }
  aux = list->first;
  aux->right->left = NULL;
  list->first = aux->right;
  item = aux->item;
  free(aux);
  list->lenght--;
  return item;
}

Item* popBack(List* list) {
  ListItem* aux;
  Item* item;
  if (list->lenght == 1) {
    item = list->first->item;
    free(list->first);
    list->first = list->last = NULL;
    return NULL;
  }
  aux = list->last;
  aux->left->right = NULL;
  list->last = aux->left;
  item = aux->item;
  free(aux);
  list->lenght--;
  return item;
}

void freeList(List* list) {
  for (unsigned i = 0; i < list->lenght; i++) {
    free(popBack(list));
  }
  free(list);
}

Item* popItem(List* list, unsigned position) {
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
  free(iterator);
  list->lenght--;
  return item;
}

Item* createItem(int pid, int priority, char** params) {
  Item* item = (Item*)malloc(sizeof(Item));
  item->pid = pid;
  item->priority = priority;
  item->params = params;
  return item;
}

void printAll(List* list) {
  ListItem* aux = list->first;
  printf("printAll:\n");
  for (unsigned i = 0; i < list->lenght; i++) {
    printf("pid %d = %d\n", i, aux->item->pid);
    aux = aux->right;
  }
}

/*
int main() {
  List* myList = createList();
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
  pushBack(myList, item1);
  pushBack(myList, item2);
  pushBack(myList, item3);
  pushBack(myList, item4);
  pushBack(myList, item5);
  pushBack(myList, item6);
  pushBack(myList, item7);

  printAll(myList);
  printf("list length = %d\n", myList->lenght);
  printf("\n");

  printf("Init\n");
  printf("first = %d\n", myList->first->item->pid);
  printf("last = %d\n", myList->last->item->pid);

  if ((aux = popFront(myList)) != NULL) {
    printf("\npopFront\n");
    printf("first = %d\n", myList->first->item->pid);
    printf("last = %d\n", myList->last->item->pid);
    printf("aux = %d\n", aux->pid);
  } else {
    printf("\npopFront failed!\n");
  }

  free(aux);
  if ((aux = popBack(myList)) != NULL) {
    printf("\npopBack\n");
    printf("first = %d\n", myList->first->item->pid);
    printf("last = %d\n", myList->last->item->pid);
    printf("aux = %d\n", aux->pid);
  } else {
    printf("\npopBack failed!\n");
  }

  pushFront(myList, item8);
  printf("\npushFront\n");
  printf("first = %d\n", myList->first->item->pid);
  printf("last = %d\n", myList->last->item->pid);

  pushBack(myList, item9);
  printf("\npushBack\n");
  printf("first = %d\n", myList->first->item->pid);
  printf("last = %d\n", myList->last->item->pid);

  free(aux);
  if (insertItem(myList, 2, item10) == 0) {
    printf("\ninsertItem\n");
    printf("first = %d\n", myList->first->item->pid);
    printf("last = %d\n", myList->last->item->pid);
    if ((aux = getItem(myList, 2)) != NULL) {
      printf("getItem\n");
      printf("aux = %d\n", aux->pid);
    } else {
      printf("getItem failed!\n");
    }
  } else {
    printf("\ninsertItem failed!\n");
  }

  if ((aux = popItem(myList, 2)) != NULL) {
    printf("\npopItem\n");
    printf("first = %d\n", myList->first->item->pid);
    printf("last = %d\n", myList->last->item->pid);
    printf("aux = %d\n", aux->pid);
  } else {
    printf("\npopItem failed!\n");
  }

  printf("\n");
  printAll(myList);
  printf("list length = %d\n", myList->lenght);

  free(aux);
  freeList(myList);
  return 0;
}
*/
