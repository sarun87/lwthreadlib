/*
Single Author info:
asriram Arun Sriraman
Group info:
asriram Arun Sriraman
shyamp	Shyam Prasad
cvsane	Chinmay	Sane
*/
#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include "mythread.h"


/*
 * Queue Element
 */
struct queueNode
{
	tcb *threadBlock; // Pointer to thread control blcok
	struct queueNode *prevPtr; // Link to next node
	struct queueNode *nextPtr; // Link to previous node
};

/*
 * Queue data structure containing front and rear pointers
 */
struct Queue
{
	struct queueNode *front; // head of the queue
	struct queueNode *rear;  // the tail of the queue
};

/*
 * Queue for threads present in the ready state.
 */
struct Queue *readyQueue=NULL;

/*
 * Queue for threads in the wait state
 */
struct Queue *waitQueue=NULL;

/*
 * List of threads that have completed execution.
 */
struct Queue *exitList = NULL;


/*
 * Adds the thread control block to the tail of the queue. The queue
 * to whcih the node is added is specified by myQueue. Address of the
 * queue is passed by the caller.
 */
void enqueue(struct Queue **myQueue, tcb *tBlock )
{
	// Check if queue has been created. If not, create it.
	if(*myQueue == NULL){
		*myQueue = (struct Queue*) malloc(sizeof(*myQueue));
		(*myQueue)->front = NULL;
		(*myQueue)->rear = NULL;
	}
	// Create a new node and add tcb to it.
	struct queueNode* newNode = malloc(sizeof(struct queueNode*));
	newNode->threadBlock = tBlock;

	// Add the node to the queue.
	if((*myQueue)->front == NULL && (*myQueue)->rear == NULL){
		newNode->nextPtr = NULL;
		newNode->prevPtr = NULL;
		(*myQueue)->front = (*myQueue)->rear = newNode;
	}else
	{
		newNode->prevPtr = NULL;
		newNode->nextPtr = (*myQueue)->rear;
		(*myQueue)->rear->prevPtr = newNode;
		(*myQueue)->rear = newNode;
	}
}

/*
 * Removes the thread control block from the head of the queue.
 */
tcb* dequeue(struct Queue** myQueue){

	struct queueNode* poppedNode = NULL;
	tcb* returnData = NULL;
	// If queue is empty/not created, return null
	if(*myQueue == NULL){
		return NULL;
	}
	// This condition should not ideally happen. If occured, error
	if((*myQueue)->front == NULL){
		// Error - No contents in queue
	}
	else if((*myQueue)->front == (*myQueue)->rear)
	{
		// Only element-> Pop that out->
		poppedNode = (*myQueue)->front;
		(*myQueue)->front = (*myQueue)->rear = NULL;
	}
	else
	{	
		// Remove node and set respective pointers
		poppedNode = (*myQueue)->front;
		((*myQueue)->front->prevPtr)->nextPtr = NULL; //myQueue->front->nextPtr;
		(*myQueue)->front = (*myQueue)->front->prevPtr;
	}
	if(poppedNode!=NULL)
	{
		returnData = poppedNode->threadBlock;
	}
	return returnData;
}

/*
 * Returns a pointer to the head node in the queue. The queue is not
 * disturbed by this method.
 */
tcb* returnFrontNode(struct Queue **myQueue){
	tcb* frontNode = NULL;
	if(*myQueue != NULL){
		frontNode = ((*myQueue)->front)->threadBlock;
	}
	return frontNode;
}

/*
 * Returns thread control block with the threadID given by the user.
 * Function returns NULL if the element is not present in the queue.
 */
tcb* returnThreadWithTID(struct Queue **myQueue, mythread_t threadID){
	tcb* returnValue = NULL;
	struct queueNode *temp = (*myQueue)->rear;
	while(temp!=NULL) {
		if((temp->threadBlock)->tid == threadID ){
			returnValue = temp->threadBlock;
			break;
		}
		temp = temp->nextPtr;
	}
	return returnValue;
}

/*
 * Remove the thread present in the wait queue given by threadID.
 */
tcb* removeWaitThread(struct Queue **myQueue, mythread_t threadID){
	tcb* returnValue = NULL;
	struct queueNode *curr = (*myQueue)->rear;
	while(curr!=NULL){
		if((curr->threadBlock)->tid == threadID){
			returnValue = curr->threadBlock;
			if(curr == (*myQueue)->rear){
				(*myQueue)->rear = curr->nextPtr;
			}
			if(curr == (*myQueue)->front){
				(*myQueue)->front = curr->prevPtr;
			}
			if(curr->nextPtr != NULL){
			(curr->nextPtr)->prevPtr = curr->prevPtr;}
			if(curr->prevPtr != NULL){
			(curr->prevPtr)->nextPtr = curr->nextPtr;}
			break;
		}
		curr = curr->nextPtr;
	}
	if(returnValue != NULL){
		free(curr);
	}
	return returnValue;
}
#endif /* QUEUE_H_ */
