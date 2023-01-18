#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
cc -I. list.c -o list -Wall -Wextra -pedantic -g -O0 -M -MT dep/list.d -MT \
bin/list.o -MP -MF dep/list.d -ggdb -DDEBUG &&
cc -I. list.c -c -Wall -pedantic -Wextra -ggdb -O0 -DDEBUG -g -o bin/list.o
*/

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

Item* createItem(long pid, int priority, char programName[30], char** params) {
  Item* item = (Item*)malloc(sizeof(Item));
  item->parent = NULL;
  item->pidVirtual = pid;
  strcpy(item->programName, programName);
  item->priority = priority;
  item->params = params;
  item->quantumTimes = 0;  // numero de quantums com processo running
  time_t now;
  now = time(0);
  if ((item->startTime = localtime(&now)) == NULL) {
    printf("Erro ao extrair tempo\n");
  }
  item->dynamicCriteria = 0;
  return item;
}

void printAll(ProcList* list) {
  ListItem* aux = list->first;
  printf("printAll:\n");
  for (unsigned i = 0; i < list->lenght; i++) {
    printf("pid %d = %ld\n", i, aux->item->pidVirtual);
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
