/*
 * Thread_join, lock, and cv test code.
 */
#include <types.h>
#include <lib.h>
#include <thread.h>
#include <synch.h>
#include <test.h>
#include <clock.h>


/*global values for lock testing*/
static volatile unsigned long ltval1;
static volatile unsigned long ltval2;
static volatile unsigned long ltval3;

struct lock *lock, *lockb;
struct cv *cv;

#define NTHREADS  8


static
void
loudthread(void *junk, unsigned long num)
{
	int ch = '0' + num;
	int i;

	(void)junk;

	for (i=0; i<120; i++) {
		putch(ch);
	}
	thread_exit();
}

/*
 * The idea with this is that you should see
 *
 *   01234567 <pause> 01234567
 *
 * (possibly with the numbers in different orders)
 *
 * The delay loop is supposed to be long enough that it should be clear
 * if either timeslicing or the scheduler is not working right.
 */
static
void
quietthread(void *junk, unsigned long num)
{
	int ch = '0' + num;
	volatile int i;

	(void)junk;

	putch(ch);
	for (i=0; i<200000; i++);
	putch(ch);

	thread_exit();
}

/*Modified version of built in test using thread_fork_join and
thread_join.*/

static
void
runthreads(int doloud)
{
	char name[16];
	int i, result, id;
    struct thread *threads[8];

	for (i=0; i<NTHREADS; i++) {
		snprintf(name, sizeof(name), "threadtest%d", i);
		result = thread_fork_join(name, NULL,
				     doloud ? loudthread : quietthread,
				     NULL, i,&(threads[i]));
		if (result) {
			panic("threadtest: thread_fork failed %s)\n",
			      strerror(result));
		}
	}

	for (i=0; i<NTHREADS; i++) {
		id = thread_join(threads[i]);
        kprintf("%d thread joined.\n", id);
	}
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
	
    if (lock==NULL) {
		lock = lock_create("testlocka");
		if (lock == NULL) {
			panic("asst1_locktest: lock_create failed\n");
		}	
    }

    /*create five threads to test locks*/

    for (i=0; i<5; i++) {
		result = thread_fork("locktest", NULL, lock_thread,
				     NULL, i);
		if (result) {
			panic("locktest: thread_fork failed: %s\n",
			      strerror(result));
		}
	}
	
}

static
void
delay_thread_signal(void *junk, unsigned long num)
{
	volatile int i;
    struct timespec time, timeEnd;
	(void)junk;

	/*Long delay to show the thread calling wait calls wait and resumes 
    after signal*/
    gettime(&time);	
    for (i=0; i<9000000; i++){	
        num=num+i;
    }
    gettime(&timeEnd);
    timespec_sub(&timeEnd, &time, &timeEnd);
    kprintf("Time in delay thread = %d\n", timeEnd.tv_nsec);
    cv_signal(cv, lockb);
}

static
void
delay_thread_broadcast(void *junk, unsigned long num)
{
	volatile int i;
    struct timespec time, timeEnd;
	(void)junk;

	/*Long delay to show the thread calling wait calls wait and resumes 
    after signal*/
    gettime(&time);	
    for (i=0; i<9000000; i++){	
        num=num+i;
    }
    gettime(&timeEnd);
    timespec_sub(&timeEnd, &time, &timeEnd);
    kprintf("Time in delay thread = %d\n", timeEnd.tv_nsec);
    cv_broadcast(cv, lockb);
}

static 
void 
asst1_cvtest(void)
{

    int result;
    int i = 1;        
    cv = cv_create("testcv");
    lockb = lock_create("testlockb");     
    struct timespec time, timeEnd;
    
    result = thread_fork("cvtest", NULL, delay_thread_signal,
				     NULL, i);
		if (result) {
			panic("locktest: thread_fork failed: %s\n",
			      strerror(result));
		}
    
    lock_acquire(lockb);
    gettime(&time);
    cv_wait(cv, lockb);
    gettime(&timeEnd);
    timespec_sub(&timeEnd, &time, &timeEnd);
    kprintf("Time elapsed in waiting thread = %d.  Successful wait and signal.\n", timeEnd.tv_nsec); 
    lock_release(lockb);  

    result = thread_fork("cvtest", NULL, delay_thread_broadcast,
				     NULL, i);
		if (result) {
			panic("locktest: thread_fork failed: %s\n",
			      strerror(result));
		}
    lock_acquire(lockb);
    gettime(&time);
    cv_wait(cv, lockb);
    gettime(&timeEnd);
    timespec_sub(&timeEnd, &time, &timeEnd);
    kprintf("Time elapsed in waiting thread = %d.  Successful wait and broadcast.\n", timeEnd.tv_nsec); 
    lock_release(lockb);  
    cv_destroy(cv);
    lock_destroy(lockb);  
		   
		

}

int
asst1_tests(int nargs, char **args)
{
	(void)nargs;
	(void)args;

	/*Lock test*/
    kprintf("Starting lock test...\n");
    asst1_locktest();
    kprintf("Lock test done.\n");

    /*CV test*/
    kprintf("Starting CV test...\n");
    asst1_cvtest();
    kprintf("CV test done.\n");

    /*thread_join test*/
    kprintf("Starting thread_join test.\n");
    runthreads(1);
    kprintf("\nthread_join test done.\n");

    


	return 0;
}
