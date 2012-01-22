/*
 * checkQueueImpl.c
 *
 *  Created on: Jan 21, 2012
 *      Author: arun
 */
#include<stdio.h>
#include<stdlib.h>
#include "queueImpl.h"

int main(){
	int choice;

	while(1){
	printf("\n\n========= MENU=========\n");
	printf("1) Add to queue\n");;
	printf("2) Remove from queue\n");
	printf("3) Display queue\n");
	printf("4) Quit\n");
	printf("Enter Choice:");
	scanf("%d",&choice);
	switch(choice){
	case 1: printf("Enter data: number:");
			scanf("%d",&choice);
			enqueue(&readyQueue,choice);
			break;
	case 2: printf("Deleting node with data:%d\n", dequeue(&readyQueue));
			break;
	case 3: printQueue(&readyQueue);
			break;
	case 4: return 0;
			break;
	default: break;

	}
	}
	return 0;
}
