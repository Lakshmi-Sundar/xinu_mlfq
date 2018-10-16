/* ready.c - ready */

#include <xinu.h>

qid16	readylist;			/* Index of ready list		*/
qid16   priolevel1;
qid16   priolevel2;
qid16   priolevel3;

/*------------------------------------------------------------------------
 *  ready  -  Make a process eligible for CPU service
 *------------------------------------------------------------------------
 */
status	ready(
	  pid32		pid		/* ID of process to make ready	*/
	)
{
	register struct procent *prptr;

	if (isbadpid(pid)) {
		return SYSERR;
	}

	/* Set process state to indicate ready and add to ready list */

	prptr = &proctab[pid];
	prptr->prstate = PR_READY;
	if(prptr->user_proc) {
		//kprintf("prio level: %d, pid: %d\n", prptr->prpriolvl, pid);
		insert(pid, prptr->prpriolvl, prptr->prprio);
	}
	else
		insert(pid, readylist, prptr->prprio);
	resched();

	return OK;
}
