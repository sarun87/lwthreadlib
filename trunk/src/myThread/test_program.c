/*
 Author : Chinmay Sane (cvsane)
 Group  : Arun Sriraman (asriram)
	  Shyam Prasad (shyamp)
	  Chinmay Sane (cvsane)
*/	  

#include <stdio.h>
#include <stdlib.h>
#include <mythread.h>
#include <sys/types.h>

mythread_key_t key1 = 1;
mythread_key_t key2 = 2;
mythread_t threads[120];


void print_data(int j)
{
	if(j == 100)
	{
		int *intdata1 = (int *)mythread_getspecific(key1);
		if(intdata1 == NULL)
			return;
		printf("\nFrom thread%d, for key1 printing data : %d %d\n\n",j,intdata1[0],intdata1[1]);
	}
	else if(j == 101)
	{
		int *intdata2 = (int *)mythread_getspecific(key2);
		if (intdata2 == NULL)
			return;
		printf("\nFrom thread%d, for key2 printing data : %d %d\n\n",j,intdata2[0],intdata2[1]);
		
	}
	else
		printf("No data to display\n");
}


void threadfunc (void *i)
{



	unsigned int j=0;
	int retval;
	int *intdata1 = malloc(sizeof(int) * 2);
	int *intdata2 = malloc(sizeof(int) * 2);
	
	intdata1[0] = 12;
	intdata2[0] = 50;
	intdata1[1] = 13;
	intdata2[1] = 51;

	//Test case 2
	if(*(int *)i == 0)
	{
		j=*(int *)i+1;
		printf("New 'thread%d' created with threadID : %d\n\n",*(int *)i,mythread_self());
		printf("In child (thread%d), creating grandchild (thread%d)\n",*(int *)i,j);
		mythread_create(&threads[j], NULL, (void* (*)(void*))&threadfunc,(void *)&j);
		mythread_yield();
		printf("Child thread(thread%d) regained CPU control from grandchild thread(thread%d), \
now child (thread%d) yeilding so that grandchild (thread%d) can exit\n\n",*(int *)i,j,*(int *)i,j);
		mythread_yield();
		printf("Exiting child thread (thread%d)\n",*(int*)i);
		mythread_exit(&retval);
	}


	//Test case 3
	else if (*(int *)i == 100)
	{
		printf("New 'thread%d' created with threadID : %d\n\n",*(int *)i,mythread_self());
		printf("Associating key1 with threadspecific data for thread%d\n",*(int *)i);
		mythread_setspecific(key1,(void *)intdata1);
		print_data(*(int *)i);
		printf("Yielding from thread%d\n\n",*(int *)i);
		mythread_yield();
		printf("Exiting child thread (thread%d)\n",*(int *)i);
		mythread_exit(&retval);
		
	}
	else if (*(int *)i == 101)
	{
		printf("New 'thread%d' created with threadID : %d\n\n",*(int *)i,mythread_self());
		printf("Associating key2 with threadspecific data for thread%d\n",*(int *)i);
		mythread_setspecific(key2,(void *)intdata2);
		print_data(*(int *)i);

		printf("Yielding from thread%d\n\n",*(int *)i);
		mythread_yield();
		printf("Exiting child thread (thread%d)\n",*(int *)i);
		mythread_exit(&retval);
		
	}


	//Test case 1
	else 
	{
		printf("New 'thread%d' created with threadID : %d\n",*(int *)i,mythread_self());
		printf("Yielding CPU control from thread%d\n\n",*(int *)i);
		mythread_yield();
		printf("Regained CPU control for thread%d, now exiting the thread\n",*(int *)i);
		mythread_exit(&retval);
	}
}


void destructor1(void *arg)
{
	printf("in fuction destructor1, freeing data related to key1\n");
	int *array = (int *) arg;
	if(array != NULL)
		free(array);
}

void destructor2(void *arg)
{
	printf("in fuction destructor2, freeing data related to key2\n");
	int *array = (int *) arg;
	if(array != NULL){
		free(array);
		array = NULL;
	}
}


void test_1()
{
	printf("\n\n	--------------Test Case 1---------------\n \
	Create, yeild, exit and join on 3 threads using following functions\n \
	mythread_create\n\
        mythread_self\n\
	mythread_yield\n\
	mythread_exit\n\
	mythread_join\n\n");
	unsigned int a[4], return_value;
	void *status;
	int i;
	
	// Create 3 threads
	for (i=0; i < 3; i++)
	{
		a[i] = i;
		printf("In main, creating thread%d\n",i+1);
		return_value = mythread_create(&threads[i], NULL, (void* (*)(void*)) &threadfunc,(void *)& a[i]);
		if(return_value != 0)
		{
			printf("Error in creating thread%d\n",i+1);
		}
	}
	printf("\n");
	//Join on 3 threads
	for (i=0; i < 3; i++)
	{
		return_value = mythread_join(threads[i], &status);
		printf("In main, joining on thread%d\n",i+1);
		if(return_value != 0)
		{
			printf("Error in joining on thread%d, with error %d\n",i,*(int *)status);
		}
	}
	printf("\nTest case 1 PASS\nMain thread Successfully joined on all threads\n\n");
}


void test_2()
{	
	int i = 0;
	void *status;
	printf("\n\n	------------Test case 2-----------\n\
	Create a child thread in main, and grandchild thread in child thread and yield once from both\n\n");
	printf("In main, creating child thread%d\n",i);
	mythread_create(&threads[i], NULL, (void* (*)(void*))threadfunc,(void *)&i);
	mythread_join(threads[i],&status);
	printf("In main, joining on child thread (thread%d) with thread ID:%d\n",i, threads[i]);
	printf("\nTest case 2 PASS\nBoth child and grandchild thread executed and exited\n\n");
}


void test_3()
{
	printf("\n\n	-------------Test Case 3-------------\n\
	Create two keys, create thread100 to associate data with key1, and thread101 to associate data with key2\n\
	Use Thread-key related functions to operate on key-data associations\n\n");
	int i = 100;
	int j = 101;
	int index;
	void *status;
	//Create key1 and key2
	printf("In main, pthread_key_create for key1 and key2\n");
	mythread_key_create(&key1, (void (*)(void *)) &destructor1);
	mythread_key_create(&key2, (void (*)(void *)) &destructor2);
	
	//Create two threads, one will use only key1, other will use both key1 and key2
	printf("In main, creating child thread%d, this will use only key1\n",i);
	mythread_create(&threads[i], NULL, (void* (*)(void*))threadfunc,(void *)&i);
	printf("In main, creating child thread%d, this will use both key1 and key2\n",j);
	mythread_create(&threads[j], NULL, (void* (*)(void*))threadfunc,(void *)&j);

	for (index=100; index < 102; index++)
	{
		mythread_join(threads[index], &status);
		printf("In main, joining on thread%d\n",index);
	}
	//sleep(10);
	printf("In main, pthread_key_delete for key1 and key2\n");
	mythread_key_delete(key1);
	mythread_key_delete(key2);
	printf("\nTest case 3 PASS\nSuccessfully used thread-key related functions\n\n");
}




int main ()

{

	//Test case 1 : Creating 5 threads, joining on all 5
	//Functions   : mythread_create, mythread_self, mythread_yield, mythread_exit, mythread_join
	test_1();

	//Test case 2: Create 1 child thread, and spawn grandchild thread from child.
	//             Yield from both child and grandchild thread once.
	test_2();

	//Test case 3: Create keys, associate them with thread specific data, 
	//             get threadspecific data from keys and delete keys.
	//Functions  : mythread_key_create, mythread_setspecific, mythread_getspecific, mythread_key_delete
	test_3(); 
	exit(0);       
}
