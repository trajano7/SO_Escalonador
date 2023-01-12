/**
 * Queue FIFO structure.
 * @param capacity Max capacity set at creation.
 * @param size Current size (amount of items).
 * @param front First item in queue (next to dequeue).
 * @param rear Last item in queue (last enqueued.
 * @param items Items in the queue.
 */
struct Queue {
  unsigned capacity;
  unsigned size;
  int front;
  int rear;
  int* items;
};

/**
 * @param capacity Queue's max capacity.
 */
struct Queue* createQueue(unsigned);

/**
 * @param queue Queue to check.
 */
int isFull(struct Queue*);

/**
 * @param queue Queue to check.
 */
int isEmpty(struct Queue*);

/**
 * @param queue Queue to increment.
 * @param item Item to put in queue.
 */
void enqueue(struct Queue*, int);

/**
 * @param queue Queue to decrement.
 */
int dequeue(struct Queue*);

/**
 * @param queue Queue to check.
 */
int front(struct Queue*);

/**
 * @param queue Queue to check.
 */
int rear(struct Queue*);
