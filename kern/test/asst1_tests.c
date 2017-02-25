/*
 * Thread_join test code.
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>

#define NTHREADS  1


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

    /*Lock and cv custom test still need to be implemented*/
	return 0;
}

