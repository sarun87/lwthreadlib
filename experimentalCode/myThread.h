/*
 * mythread.h -- interface of user threads library
 */

#ifndef MYTHREAD_H
#define MYTHREAD_H

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#include <sys/types.h>
#include <sched.h>
#include <malloc.h>
#include <sys/wait.h>
#include <signal.h>

//#include "futex.h"
#define STACK_SIZE 1024*64

/* Keys for thread-specific data */
typedef unsigned int mythread_key_t;

/* add your code here */
typedef unsigned int mythread_t;
typedef unsigned int uint32_t;
typedef unsigned int mythread_attr_t;

struct TCB
{
	mythread_t tid;	// Thread ID
	mythread_t pid; // Parent ID
	void * threadStack; // Stack Pointer
	uint32_t status; // Thread RUNNING, READY, EXITTED, WAITING
	// Add context data - registers

};
typedef struct TCB tcb;

tcb myQueue[10];

/*
 * mythread_self - thread id of running thread
 */
mythread_t mythread_self(void);
void* stack;
/*
 * mythread_create - prepares context of new_thread_ID as start_func(arg),
 * attr is ignored right now.
 * Threads are activated (run) according to the number of available LWPs
 * or are marked as ready.
 */
int mythread_create(mythread_t *new_thread_ID,
		    mythread_attr_t *attr,
		    void * (*start_func)(void *),
		    void *arg)
{

	         pid_t pid;

	         // Allocate the stack
	         stack = malloc( STACK_SIZE );
	         if ( stack == 0 )
	         {
	                 perror( "malloc: could not allocate stack" );
	                 exit( 1 );
	         }

	         printf( "Creating child thread\n" );

	         // Call the clone system call to create the child thread
	         pid = clone((int (*)(void*)) start_func, stack + STACK_SIZE,
	        		 CLONE_VM, arg );
	         if ( pid == -1 )
	         {
	                 perror( "clone" );
	                 exit( 2 );
	         }
	         *new_thread_ID = pid;
}

/*
 * mythread_yield - switch from running thread to the next ready one
 */
int mythread_yield(void);

/*
 * mythread_join - suspend calling thread if target_thread has not finished,
 * enqueue on the join Q of the target thread, then dispatch ready thread;
 * once target_thread finishes, it activates the calling thread / marks it
 * as ready.
 */
int mythread_join(mythread_t target_thread, void **status);

/*
 * mythread_exit - exit thread, awakes joiners on return
 * from thread_func and dequeue itself from run Q before dispatching run->next
 */
void mythread_exit(void *retval)
{
	free(stack);
}

/*
 * mythread_key_create - thread-specific data key creation
 * The  mythread_key_create()  function shall create a thread-specific data
 * key visible to all threads in  the  process.  Key  values  provided  by
 * mythread_key_create()  are opaque objects used to locate thread-specific
 * data. Although the same key value may be used by different threads, the
 * values  bound  to  the key by mythread_setspecific() are maintained on a
 *per-thread basis and persist for the life of the calling thread.
 */
int mythread_key_create(mythread_key_t *key, void (*destructor)(void*));

/*
 * mythread_key_delete - thread-specific data key deletion
 * The  mythread_key_delete()  function shall delete a thread-specific data
 * key previously returned by  mythread_key_create().  The  thread-specific
 * data  values  associated  with  key  need  not  be  NULL  at  the  time
 * mythread_key_delete() is called.  It is the responsibility of the appli‐
 * cation  to  free any application storage or perform any cleanup actions
 * for data structures related to the deleted key  or  associated  thread-
 * specific data in any threads; this cleanup can be done either before or
 * after mythread_key_delete() is called. Any attempt to use key  following
 * the call to mythread_key_delete() results in undefined behavior.
 */
int mythread_key_delete(mythread_key_t key);

/*
 * mythread_getspecific - thread-specific data management
 * The mythread_getspecific() function shall  return  the  value  currently
 * bound to the specified key on behalf of the calling thread.
 */
void *mythread_getspecific(mythread_key_t key);

/*
 * mythread_setspecific - thread-specific data management
 * The  mythread_setspecific()  function  shall associate a thread-specific
 * value with a key obtained via a previous call to  mythread_key_create().
 * Different threads may bind different values to the same key. These val‐
 * ues are typically pointers to blocks of  dynamically  allocated  memory
 *that have been reserved for use by the calling thread.
 */
int mythread_setspecific(mythread_key_t key, const void *value);


#endif /* MYTHREAD_H */
