#include "queue.h"

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

Queue* createQueue(unsigned capacity) {
  Queue* queue = (Queue*)malloc(sizeof(Queue));
  queue->capacity = capacity;
  queue->front = queue->size = 0;
  queue->rear = capacity - 1;
  queue->items = (int*)malloc(queue->capacity * sizeof(int));
  return queue;
}

int isFull(Queue* queue) {
  return (queue->size == queue->capacity);
}

int isEmpty(Queue* queue) {
  return (queue->size == 0);
}

void enqueue(Queue* queue, int item) {
  if (isFull(queue)) {
    return;
  }
  queue->rear = (queue->rear + 1) % queue->capacity;
  queue->items[queue->rear] = item;
  queue->size = queue->size + 1;
}

int dequeue(Queue* queue) {
  if (isEmpty(queue)) {
    return INT_MIN;
  }
  int item = queue->items[queue->front];
  queue->front = (queue->front + 1) % queue->capacity;
  queue->size = queue->size - 1;
  return item;
}

int front(Queue* queue) {
  if (isEmpty(queue))
    return INT_MIN;
  return queue->items[queue->front];
}

int rear(Queue* queue) {
  if (isEmpty(queue))
    return INT_MIN;
  return queue->items[queue->rear];
}

/*
int main() {
  Queue* queue = createQueue(1000);
  printf("Front item is %d\n", front(queue));
  printf("Rear item is %d\n", rear(queue));
  enqueue(queue, 10);
  enqueue(queue, 20);
  enqueue(queue, 30);
  enqueue(queue, 40);
  printf("%d dequeued from queue\n\n", dequeue(queue));
  printf("Front item is %d\n", front(queue));
  printf("Rear item is %d\n", rear(queue));
  return 0;
}
*/
