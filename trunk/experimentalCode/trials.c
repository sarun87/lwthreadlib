/*
 ============================================================================
 Name        : trials.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <malloc.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include "myThread.h"

int myGlobalVariable = 10;

// The child thread will execute this function
int threadFunction( void* argument )
{
		int id = *(int*)argument;
		printf( "child thread hello. Thread Number: %d GV: %d\n", *(int*)argument,myGlobalVariable);
		if( *(int*)argument == 1){
			myGlobalVariable = *(int*)argument;
		}
		printf( "child thread changed. Thread Number: %d GV: %d\n", *(int*)argument,myGlobalVariable);
		sleep(20);
		printf("Thread %d done\n",id);
        return 0;
}

int main()
{

		mythread_t pid[5];
		int index;
		int ids[6] = {1,2,3,4,5,6};
		for(index = 0; index < 5; ++index){
			mythread_create(&pid[index],NULL,(void *(*)(void*)) threadFunction, &ids[index]);
			sleep(4);
		}

         printf( "Child thread returned and stack freed.\n" );
         sleep(40);
         return 0;
}















































/*
#include <stdio.h>
#include <stdlib.h>
//#include <pthread.h>
#include "myThread.h"

	int **abc;

	int mno(void *ab)
	{
		write(1,"From Thread 1",50);
		return 0;
		//mythread_exit(NULL);
	}
int main(void) {
//	pthread_t m;
//	int index;
//	long id = 1;
//	puts("!!!Hello World!!!"); /* prints !!!Hello World!!!
//
//	abc = calloc(10,sizeof(int*));
//	for(index = 0; index < 10; ++index){
//		abc[index]= calloc(10, sizeof(int));
//		//abc[index][0] = 1;
//		//abc[index][1] = 2;
//	}
//
//	pthread_create(&m, NULL, mno, (void *) &id);
//	pthread_join(m, NULL);
//	puts("!!!Hello World!!!"); prints !!!Hello World!!!
//	printf("\n%d  %d",abc[0][0],abc[5][1]);
//	return EXIT_SUCCESS;
	mythread_t m;
	void* (*ptr)(void*) = mno;
	long id =1;
	int pid;
	void * stack = malloc(16534);
	write(1,"Before my thread Create!",50);
    mythread_create(&m, NULL, mno, (void*) &id);
	pid = clone(&mno,(char*)stack+16534,SIGCHLD | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_VM, 0);
	waitpid( pid, 0, 0 );
	sleep(2);
	write(1,"Main Thread!!",50);
	return 0;
}


*/
