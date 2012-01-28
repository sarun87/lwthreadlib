/*
Single Author info:
asriram Arun Sriraman
shyamp	Shyam Prasad
Group info:
asriram Arun Sriraman
shyamp	Shyam Prasad
cvsane	Chinmay	Sane
*/

/* System Includes*/
#include <sched.h>
#include <stdlib.h>
#include <signal.h>

/* Local Includes */
#include "mythread.h"
#include "queue.h"
#include "mythread_keys.h"


/* Pointers. For idle thread, mainthread and new tcb's */
tcb *idleThread;
tcb *mainThread;
tcb *newTcb;

/*
 * @Author: Shyam Prasad
 * Idle thread method. This method will be executed by the library internal
 * library thread. The only functionality of this thread is to check for the
 * next ready thread for execution and yield to that thread.
 * @arg - Input arguments - NULL 
 * @return - (void)
 */
void runIdleThread(void *arg) {
	while (1) {
		// Check if there is a thread waiting in the front of the ready queue.
		if (readyQueue->front != NULL) {
			// Dequeue. This is the idle thread tcb.
			tcb * thisThread = dequeue(&readyQueue);
			tcb * zombieThread = NULL;
			// Check if there is another thread ready to run in the ready queue.
			if (!returnFrontNode(&readyQueue)) {
				// No other thread present in ready queue. This is the last thread.
				// Clear this tcb and quit.
				enqueue(&exitList, thisThread);
				while ((zombieThread = dequeue(&exitList)) != thisThread) {
					zombieThread->arg = NULL;
					zombieThread->start_func = NULL;
					free(zombieThread->threadStack);
					zombieThread->threadStack = NULL;
					free(zombieThread);
				}
				exit(0);
			}
			// There is another ready thread. Enqueue the idle thread, 
			// start the thread that is present in the head of the queue.
			enqueue(&readyQueue, thisThread);

			tcb* threadToExecute = returnFrontNode(&readyQueue);
			futex_up(&(threadToExecute->myFutex));
			futex_down(&(thisThread->myFutex));
		}
	}

}

/* 
 * @Author: Arun Sriraman
 * The wrapper method that is called when a new thread is created. This met
 * hod is given to the clone system call. The wrapper method yields before 
 * calling the user method with the specific arguments. Once the user method
 * executes, the mythread_exit() is called.
 */
void wrapper(void *arg) {
	tcb *target = NULL;
	tcb *myThreadTcb = (tcb*) arg;
	// Sleep self thread
	futex_down(&(myThreadTcb->myFutex));

	// Call user function for execution.
	(myThreadTcb->start_func)(myThreadTcb->arg);

	// After user function has executed.
	// Remove the tcb from the ready queue
	if ((target = returnThreadWithTID(&exitList, myThreadTcb->tid)) == NULL) {
		mythread_exit(NULL);
	}
}

/*
 * @Author: Shyam Prasad
 *
 */
mythread_t mythread_self(void) {
	if (readyQueue != NULL) {
		tcb* runningThread = returnFrontNode(&readyQueue);
		if (runningThread != NULL) {
			return (runningThread->tid);
		}
	}
	return -1;
}

/*
 * @Author: Arun Sriraman
 *
 */
int mythread_create(mythread_t *new_thread_ID, mythread_attr_t *attr,
		void * (*start_func)(void *), void *arg) {
	mythread_t tid;
	unsigned int STACK_SIZE;

	// Check if stack size has been passed. If not, used the default
	// value SIGSTKSZ
	if(attr!= NULL)
	{
		STACK_SIZE = *attr;
	}
	else
	{
		STACK_SIZE = SIGSTKSZ;
	}
	// initial library setup. Check if queue has been created.
	if (readyQueue == NULL) {
		// There is no idle thread. Initialization process.
		// Get tcb for main thread
		mainThread = malloc(sizeof(*mainThread));
		futex_init(&(mainThread->myFutex), 0);
		mainThread->tid = getpid();
		mainThread->start_func = NULL;
		mainThread->threadStack = NULL;
		mainThread->status = READY;
		mainThread->retval = NULL;
		enqueue(&readyQueue, mainThread);

		// Create Idle thread and add to queue.
		idleThread = malloc(sizeof(*idleThread));
		futex_init(&(idleThread->myFutex), 0);
		idleThread->tid = 0;
		idleThread->threadStack = malloc(STACK_SIZE);
		posix_memalign((void**)&idleThread->threadStack, 8, STACK_SIZE);
		idleThread->start_func = (void* (*)(void *)) &runIdleThread;
		idleThread->arg = 0;
		idleThread->status = READY;
		idleThread->joinChildid = -1;
		idleThread->retval = NULL;
		enqueue(&readyQueue, idleThread);

		// Clone - Idle thread
		tid = clone((int(*)(void *)) &wrapper,
				idleThread->threadStack + STACK_SIZE-sizeof(sigset_t), CLONE_VM,
				(void *) idleThread);
		if (tid == -1) {
			write(1, "Error! Unable to create idle Thread!", 40);
			*new_thread_ID = -1;
			return -1;
		}
		// Assign the thread id returned from clone. This is same as calling gettid() in wrapper code.
		idleThread->tid = tid;

	}

	// Normal functionality of mythread_create.
	// Call wrapper method with user function as arguments.
	// Create tcb for the calling thread.
	newTcb = malloc(sizeof(*newTcb));
	futex_init(&(newTcb->myFutex), 0);
	newTcb->tid = -1;
	newTcb->threadStack = malloc(STACK_SIZE);
	newTcb->status = READY;
	newTcb->start_func = (void* (*)(void *)) start_func;
	newTcb->arg = arg;
	newTcb->joinChildid = -1;
	newTcb->retval = NULL;
	// En-queue newTcb
	enqueue(&readyQueue, newTcb);
	// Clone - user thread
	tid = clone((int(*)(void *)) wrapper, newTcb->threadStack + STACK_SIZE - sizeof(sigset_t),
			CLONE_VM, (void*) newTcb);
	if (tid == -1) {
		write(1, "Error! Unable to create Scheduler Thread!", 40);
		*new_thread_ID = -1;
		return -1;
	}

	// Set return value with thread ID assigned by us.
	newTcb->tid = tid;
	*new_thread_ID = newTcb->tid;

	return 0;
}

/*
 * @Author: Shyam Prasad
 */
int mythread_join(mythread_t target_thread, void **status) {
	// Check if the target_thread is present in the ready queue && wait queue
	// If not present, the calling thread need not wait. Return directly to 
	// the calling method..
	tcb* target = returnThreadWithTID(&readyQueue, target_thread);
	if (target == NULL) {
		target = returnThreadWithTID(&waitQueue, target_thread);
		if (target == NULL) {
			target = returnThreadWithTID(&exitList, target_thread);
			if (target == NULL) {
				return -1;
			} else {
				*status = (void *) target->retval;
				return 0;
			}
		}
	}
	// There is a thread. Set that thread attribute so that the tread can
	// notify this thread to continue execution.
	target->joinChildid = mythread_self();

	// Add the current running thread to wait queue. Start the next ready
	// thread.
	tcb* thisThread = dequeue(&readyQueue);
	enqueue(&waitQueue, thisThread);
	tcb* threadToRun = returnFrontNode(&readyQueue);
	futex_up(&(threadToRun->myFutex));
	futex_down(&(thisThread->myFutex));

	target = returnThreadWithTID(&exitList, target_thread);
	if (target == NULL) {
		return -1;
	}
	*status = (void *) target->retval;
	return 0;
}

/*
 * @Author: Shyam Prasad
 */ 
void mythread_exit(void *retval) {
	// For a thread to exit.
	// 1) Check if the thread has to wake a thread that joined on this thread.
	//    If yes, shift the waiting thread to ready queue.
	// 2) Call all key destructors associated with the thread.
	// 3) Start the thread that is present in the head of the ready queue.

	threadDestruct* keyDestruction = NULL;
	tcb* dyingThread = dequeue(&readyQueue);
	if (dyingThread->joinChildid != -1) {
		tcb* wakeThread =
				removeWaitThread(&waitQueue, dyingThread->joinChildid);
		enqueue(&readyQueue, wakeThread);
	}

	// Call distructor set by key_create
	keyDestruction = getAndRemove(dyingThread->tid);
	while(keyDestruction!=NULL)
	{
		(keyDestruction->threadDest_func)(keyDestruction->value);
		free(keyDestruction);
		keyDestruction = getAndRemove(dyingThread->tid);
	}

	dyingThread->threadStack = NULL;
	dyingThread->start_func = NULL;
	dyingThread->retval = retval;
	enqueue(&exitList, dyingThread);

	// Execute the next ready thread - futex up
	tcb* threadToExecute = returnFrontNode(&readyQueue);
	futex_up(&(threadToExecute->myFutex));
}

/*
 * @Author: Shyam Prasad
 */
int mythread_yield(void) {
	// Remove the tcb from the head of the queue. Enqueue it back at the end.
	// Start the next thread that is present at the head of the ready queue.
	tcb* yieldThread = dequeue(&readyQueue);
	if (yieldThread == NULL) {
		return -1;
	}
	enqueue(&readyQueue, yieldThread);

	tcb* nextThreadToRun = returnFrontNode(&readyQueue);
	if (nextThreadToRun == NULL) {
		return -1;
	}
	futex_up(&(nextThreadToRun->myFutex));
	futex_down(&(yieldThread->myFutex));

	return 0;
}

