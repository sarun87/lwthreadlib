/*
 * queueImpl.h
 *
 *  Created on: Jan 21, 2012
 *      Author: arun
 */

#ifndef QUEUEIMPL_H_
#define QUEUEIMPL_H_

#include <stdio.h>

struct queueNode
{
	int x;
	struct queueNode *prevPtr;
	struct queueNode *nextPtr;
};

struct Queue
{
	struct queueNode *front;
	struct queueNode *rear;
};

struct Queue *readyQueue;
struct Queue *waitQueue;

void enqueue(struct Queue **myQueue, int x )
{
	if(*myQueue == NULL){
		*myQueue = malloc(sizeof(struct Queue*));
		(*myQueue)->front = NULL;
		(*myQueue)->rear = NULL;
	}
	struct queueNode* newNode = malloc(sizeof(struct queueNode*));
	newNode->x = x;
	if((*myQueue)->front == NULL && (*myQueue)->rear == NULL){
		newNode->nextPtr = NULL;
		newNode->prevPtr = NULL;
		(*myQueue)->front = (*myQueue)->rear = newNode;
	}else
	{
		newNode->prevPtr = NULL;
		//newNode->prevPtr = myQueue->rear->prevPtr;
		newNode->nextPtr = (*myQueue)->rear;
		//(myQueue->rear->prevPtr)->nextPtr = newNode;
		(*myQueue)->rear->prevPtr = newNode;
		(*myQueue)->rear = newNode;
	}
}

int dequeue(struct Queue** myQueue){

	struct queueNode* poppedNode = NULL;
	int returnData = -1;
	if(*myQueue == NULL){
		return -1;
	}
	if((*myQueue)->front == NULL){
		// Error - No contents in queue
	}
	else if((*myQueue)->front == (*myQueue)->rear)
	{
		// Only element-> Pop that out->
		poppedNode = (*myQueue)->front;
		(*myQueue)->front = (*myQueue)->rear = NULL;
		free(*myQueue);
		*myQueue = NULL;

	}
	else
	{
		poppedNode = (*myQueue)->front;
		((*myQueue)->front->prevPtr)->nextPtr = NULL; //myQueue->front->nextPtr;
		//(myQueue->front->nextPtr)->prevPtr = myQueue->front->prevPtr;
		(*myQueue)->front = (*myQueue)->front->prevPtr;
	}
	if(poppedNode!=NULL)
	{
		returnData = poppedNode->x;
		free(poppedNode);
	}
	return returnData;
}

void printQueue(struct Queue** myQueue){
	if((*myQueue) == NULL)
	{
		return;
	}
	struct queueNode* temp = (*myQueue)->rear;
	if((*myQueue)->front == NULL)
	{
		printf("Error! Nothing in the Queue");
	}
	while(temp!=NULL){
		printf("  %d  ", temp->x);
		temp = temp->nextPtr;
	}
}

#endif /* QUEUEIMPL_H_ */
