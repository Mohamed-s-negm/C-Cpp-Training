#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct node
{
    int val;
    struct node* next;
}node_t;

typedef struct
{
    node_t* head;
    node_t* tail;
    int size;
}queue;
int num;
void init_queue(queue* queue);
void enqueue(queue* queue, int value);
int dequeue(queue* queue);
bool is_empty(queue* queue);
void free_queue(queue* queue);

int main(){
    queue q;
    printf("Queue Implementation using Linked List\n");
    init_queue(&q);

    printf("Enter numbers to enqueue (enter -1 to stop):\n");
    while(1) {
        printf("Enter number: ");
        scanf("%d", &num);
        if(num == -1) {
            break;
        }
        enqueue(&q, num);
        printf("Enqueued: %d\n", num);
    }

    printf("\nNow dequeuing all elements:\n");
    while(!is_empty(&q)) {
        printf("Dequeued: %d\n", dequeue(&q));
    }
    printf("Dequeued: %d\n", dequeue(&q)); // should say empty

    free_queue(&q);
    return 0;
}

void init_queue(queue* queue)
{
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;
}

bool is_empty(queue* queue)
{
    return (queue->size == 0);
}

void enqueue(queue* queue, int val)
{
    node_t* newNode = (node_t*) malloc(sizeof(node_t));
    newNode->val = val;
    newNode->next = NULL;

    if(is_empty(queue))
    {
        queue->head = newNode;
        queue->tail = newNode;
    }
    else{
        queue->tail->next = newNode;
        queue->tail = newNode;
    }
    queue->size++;
}

int dequeue(queue* queue)
{
    if(is_empty(queue))
    {
        printf("Queue is empty\n");
        return -1;
    } else {
        node_t* temp = queue->head;
        int value = temp->val;
        queue->head = queue->head->next;
        free(temp);
        queue->size--;
        return value;
    }
}

void free_queue(queue* queue)
{
    while(!is_empty(queue))
    {
        dequeue(queue);
    }
}