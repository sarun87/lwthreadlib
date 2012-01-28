/*
Single Author info:
asriram Arun Sriraman
cvsane	Chinmay Sane
Group info:
asriram Arun Sriraman
shyamp	Shyam Prasad
cvsane	Chinmay	Sane
*/
#ifndef _MYTHREAD_KEYS
#define _MYTHREAD_KEYS

/* System includes*/
#include <stdio.h>
#include <sched.h>
#include <stdlib.h>

/* User header file */
#include "mythread.h"

/* 
 * Thread-Destructor - Node containing threadID
 * and corresponding key destructor to be called.
 */
typedef struct threadDestruct threadDestruct;

/*
 * Key node. Each key is identified by this node. 
 */
typedef struct keyList keyList;

/*
 * Each thread that calls setspecific will have a node
 * associated with this.
 */ 
typedef struct threadToValue threadToValue;

struct threadToValue {
	mythread_t tid; // Thread ID
	void *value; // void* that stores the value of the key-value pair
	struct threadToValue *next_thread; // Pointer to next node
};


struct keyList {
	mythread_key_t key_value; // Key ID
	threadToValue *threadList_head; // List of nodes of type threadToValue.
	void (*destructor_func)(void*) ; // Function pointer to the destructor
	struct keyList *next_key; // Pointer to next node.
};


struct threadDestruct {
	mythread_t tid; // Thread ID
	void (*threadDest_func)(void*); // Function pointer to the destructor
	void *value; //void* that stores the value of the key-value pair
	struct threadDestruct *next_destruct; // Pointer to next node.
};

/*
 * Key list start pointer.
 */
keyList* keyList_head = NULL;

/*
 * Thread destructor list start pointer
 */
threadDestruct* destructList_head = NULL;

/*
 * Method returns the key node with value key_value by searching the key list.
 */
keyList* findkey (mythread_key_t key_value)
{
	keyList* returnValue = NULL;
	keyList* current = keyList_head;

	while(current!=NULL)
	{
		if(current->key_value == key_value)
		{
			returnValue = current;
			break;
		}
		current = current->next_key;
	}
	return returnValue;  
}

/*
 * Method returns threadToValue node present for a particular key which
 * has a thread id tid. It returns a NULL if not found.
 */
threadToValue* findvalue (keyList *searchKey, mythread_t tid)
{
	threadToValue* returnValue = NULL;
	threadToValue* current = searchKey->threadList_head;
	while(current!=NULL)
	{
		if(tid == current->tid)
		{
			returnValue = current;
			break;
		}
		current = current->next_thread;
	}
	return returnValue;
} 

/*
 * Add keynode to the keyList
 */
void addToKeyList(keyList* newKey)
{
	if(keyList_head == NULL)
	{
		keyList_head = newKey;
	}
	else
	{
		newKey->next_key = keyList_head;
		keyList_head = newKey;
	}
}

/*
 * Add thread node to thread list for the key given by key.
 */ 
void addToThreadList(keyList *key, threadToValue *newThread)
{
	if (key->threadList_head == NULL)
	{
		key->threadList_head = newThread;
	}
	else
	{
		newThread->next_thread =  key->threadList_head;
		key->threadList_head = newThread;
	}
}

/*
 * Delete all the threadToNode nodes present for a given key (keyNode)
 */
void remove_threads(keyList* keyNode)
{
	threadToValue *curr = keyNode->threadList_head;
	threadToValue *prev = NULL;
	while(curr!=NULL)
	{	prev = curr;
		curr=curr->next_thread;
		free(prev);
		prev=NULL;
	}
	keyNode->threadList_head = NULL;
}

/*
 * This used by mythread_exit() to find the destructors that the 
 * thread with thread id threadID has to call before termination.
 */
threadDestruct* getAndRemove(mythread_t threadID)
{
	threadDestruct *curr = destructList_head;
	threadDestruct *prev = NULL;
	while(curr!=NULL)
	{	
		if(curr->tid == threadID)
		{   // Found. Return curr. Reassign list pointers.
			if(prev == NULL)
			{
				destructList_head = curr->next_destruct;
			}
			else
			{
				prev->next_destruct = curr->next_destruct;
			}
			return curr;
		}
		prev = curr;
		curr = curr->next_destruct;
	}
	return NULL;
}

/*
 * @Author: Arun Sriraman
 * Description: See mythread.h 
 */
int mythread_key_create(mythread_key_t *key, void (*destructor)(void*))
{
	// Check if key has already been created.
	// Multiple calls to this method will without any action.
	keyList* retFindKey = findkey(*key);
	if(retFindKey == NULL)
	{
		// If key not present in key list, create the key node.
		// Associate a key with the destructor.
		keyList* newKey = malloc(sizeof(keyList));
		if (newKey == NULL)
			return -1;
		newKey->key_value = *key;
		newKey->threadList_head=NULL;
		newKey->next_key = NULL;
		newKey->destructor_func = destructor;
		addToKeyList(newKey);
	}	
	return 0;
}

/*
 * @Author: Arun Sriraman
 * Description: See mythread.h
 */
int mythread_key_delete(mythread_key_t key)
{
		keyList* current = keyList_head;
		keyList* previous = NULL;
		while(current!=NULL)
		{
			if(current->key_value == key)
			{
				// Remove all thread specific values
				remove_threads(current);
				if(previous == NULL){
					keyList_head = current->next_key;
				}
				else
				{
					previous->next_key = current->next_key;
				}
				free(current);
				// Done with removal. Return to caller.
				return 0;
			}
			current = current->next_key;
		}
		return -1;
}

/*
 * @Author: Chinmay Sane
 * Description: See mythread.h
 */
void *mythread_getspecific(mythread_key_t key)
{
	// Find key node from Key list
	keyList* foundKey = findkey(key);
	if(foundKey!=NULL)
	{
		// Find the specific thread storage (thread node)
		threadToValue *foundThread = findvalue(foundKey,mythread_self());
		if(foundThread!=NULL){
			// Return the pointer to user storage space.
			return foundThread->value;
		}
	}
	return NULL;
}


/*
 * @Author: Chinmay Sane
 * Description: See mythread.h
 */
int mythread_setspecific(mythread_key_t key, const void *value)
{
	//Associate value with key
	keyList* retFindKey = findkey(key);
	if(retFindKey == NULL)
	{
		return -1;
	}
	mythread_t threadID = mythread_self();
	threadToValue* newNode = malloc(sizeof(threadToValue));
	newNode->tid = threadID;
	newNode->value = (void*)value;
	newNode->next_thread = NULL;
	addToThreadList(retFindKey,newNode);

	//Associate destructor function with thread
	threadDestruct *curr = malloc(sizeof(threadDestruct));
	curr->tid = threadID;
	curr->next_destruct = NULL;
	curr->value = (void*) value;
	curr->threadDest_func = retFindKey->destructor_func;

	if (destructList_head == NULL)
	{
		destructList_head = curr;
	}
	else
	{
		curr->next_destruct = destructList_head;
		destructList_head = curr;
	}
	return 0;
}



#endif
