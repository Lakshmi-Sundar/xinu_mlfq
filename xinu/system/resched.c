/* resched.c - resched, resched_cntl */

#include <xinu.h>

struct	defer	Defer;

/*------------------------------------------------------------------------
 *  resched  -  Reschedule processor to highest priority eligible process
 *------------------------------------------------------------------------
 */
void	resched(void)		/* Assumes interrupts are disabled	*/
{
	struct procent *ptold;	/* Ptr to table entry for old process	*/
	struct procent *ptnew;	/* Ptr to table entry for new process	*/
	pid32 pid_old;
	pid32 pid_new;

	/* If rescheduling is deferred, record attempt and return */

	if (Defer.ndefers > 0) {
		Defer.attempt = TRUE;
		return;
	}

	/* Point to process table entry for the current (old) process */

	ptold = &proctab[currpid];
	pid_old = currpid;

	if (ptold->prstate == PR_CURR) {  /* Process remains eligible */
		//for system process only
		if (!ptold->user_proc && (ptold->prprio > firstkey(readylist))) {
			return;
		}

		/* Old process will no longer remain current */

		ptold->prstate = PR_READY;
		if(ptold->user_proc)
			insert(currpid, ptold->prpriolvl, ptold->prprio);
		else
			insert(currpid, readylist, ptold->prprio);
	}

	/* Force context switch to highest priority ready process */

	if((firstid(readylist) == 0) && (userprcount > 0)) {
		if(nonempty(priolevel1)) {
			currpid = dequeue(priolevel1);
			kprintf("curr pid in level 1 %d\n", currpid);
			preempt = TIME_SLICE;
		}
		else if(nonempty(priolevel2)) {
			currpid = dequeue(priolevel2);
			kprintf("curr pid in level 2 %d\n", currpid);
			preempt = 2*TIME_SLICE;
		}
		else if(nonempty(priolevel3)) {
			currpid = dequeue(priolevel3);
			kprintf("curr pid in level 3 %d\n", currpid);
			preempt = 4*TIME_SLICE;
		}
	}
	else {
		currpid = dequeue(readylist);
		preempt = TIME_SLICE;
	}
	pid_new = currpid;
	ptnew = &proctab[currpid];
	ptnew->prstate = PR_CURR;
	//preempt = QUANTUM;		/* Reset time slice for process	*/
	if(pid_old != pid_new)
		kprintf("ctxsw::%d-%d\n",pid_old, pid_new);
	ctxsw(&ptold->prstkptr, &ptnew->prstkptr);

	/* Old process returns here when resumed */

	return;
}

/*------------------------------------------------------------------------
 *  resched_cntl  -  Control whether rescheduling is deferred or allowed
 *------------------------------------------------------------------------
 */
status	resched_cntl(		/* Assumes interrupts are disabled	*/
	  int32	defer		/* Either DEFER_START or DEFER_STOP	*/
	)
{
	switch (defer) {

	    case DEFER_START:	/* Handle a deferral request */

		if (Defer.ndefers++ == 0) {
			Defer.attempt = FALSE;
		}
		return OK;

	    case DEFER_STOP:	/* Handle end of deferral */
		if (Defer.ndefers <= 0) {
			return SYSERR;
		}
		if ( (--Defer.ndefers == 0) && Defer.attempt ) {
			resched();
		}
		return OK;

	    default:
		return SYSERR;
	}
}
