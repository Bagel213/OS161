/*
 * Thread_join, lock, and cv test code.
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>


/*global values for lock testing*/
static volatile unsigned long ltval1;
static volatile unsigned long ltval2;
static volatile unsigned long ltval3;

struct lock *lock;
struct cv *cv

static
void
jointhisthread(void *junk, unsigned long num)
{
	volatile int i;

	(void)junk;

	/*short delay*/
	for (i=0; i<200000; i++);	
        num=num+i;
    thread_exit(); 
}

static
void
jointhisthreadTwo(void *junk, unsigned long num)
{
	volatile int i;

	(void)junk;

	/*Long delay to show the parent is waiting after join is called*/
	for (i=0; i<9000000; i++);	
        num=num+i;
    thread_exit();
}

/*Create threads and call them to join, causing parent to wait, then when child
exits return its ID*/

static
void
runthreads(void)
{
	char name_one[7] = "thread1";
    char name_two[7] = "thread2";
    int i = 1;
    int result, result2;

	/*Create two forks (children) for testing with unique names*/
		snprintf(name_one, sizeof(name_one), "threadtest%d", i);
		result = thread_fork(name_one, NULL,
				     jointhisthread,
				     NULL, i);
		if (result) {
			panic("threadtest: thread_fork failed %s)\n",
			      strerror(result));
		}

		snprintf(name_one, sizeof(name_two), "threadtest%d", i);
		result2 = thread_fork(name_two, NULL,
				     jointhisthreadTwo,
				     NULL, i);
		if (result2) {
			panic("threadtest: thread_fork failed %s)\n",
			      strerror(result));
		}
	
    /*Call thread join and display the thread id of each child on return*/   
    result = thread_join(name_one);
    kprintf("Returned thread id:%d\n", result);
    result2 = thread_join(name_two);
    kprintf("Returned thread id:%d", result2);
    /*If return value is zero either the name was not that of a child or the child thread
      had completed prior to join being called*/
    	
	
}

/*Make changes via computations to global shared variables in multiple threads.  Lock the variables
and ensure the lock worked by doing computations and ensure other threads have not made changes
to the shared variables between the computations and a test for the proper result*/

static
void
lock_thread(void *junk, unsigned long test)
{
	int i;
	(void)junk;

	for (i=0; i<500; i++) {
		lock_acquire(lock);
		ltval1 = test;
		ltval2 = test+test;
		ltval3 = test*test;

		if (ltval2 != ltval1+ltval1) {
			kprintf("Add test fail.\n");
		}

		if (ltval3 != ltval1*ltval1) {
			kprintf("Mult test fail\n");
		}

		lock_release(lock);
	}
	
}
static
void
asst1_locktest() {
	
    int result, i;
    char lock_name[4] = "lock";
	
    if (lock==NULL) {
		lock = lock_create(lock_name);
		if (lock == NULL) {
			panic("asst1_locktest: lock_create failed\n");
		}	
    }

    /*create five threads to test locks*/

    for (i=0; i<5; i++) {
		result = thread_fork("synchtest", NULL, lock_thread,
				     NULL, i);
		if (result) {
			panic("locktest: thread_fork failed: %s\n",
			      strerror(result));
		}
	}
	
}



int
asst1_tests(int nargs, char **args)
{
	(void)nargs;
	(void)args;

	/*Call function to test thread join with two threads*/
	kprintf("Starting thread_join test...\n");
	runthreads();
	kprintf("\nThread_join test done.\n");

    /*Lock test*/
    kprintf("Starting lock test...\n");
    asst1_locktest();
    kprintf("Lock test done.\n");

    


	return 0;
}
