/* clkhandler.c - clkhandler */

#include <xinu.h>

/*------------------------------------------------------------------------
 * clkhandler - high level clock interrupt handler
 *------------------------------------------------------------------------
 */
void	clkhandler()
{
	static	uint32	count1000 = 1000;	/* Count to 1000 ms	*/
	struct procent *prptr;
	prptr = &proctab[currpid];
	pid32 pid;
	int i;

	/* Decrement the ms counter, and see if a second has passed */

	if((--count1000) <= 0) {

		/* One second has passed, so increment seconds count */

		clktime++;

		/* Reset the local ms counter for the next second */

		count1000 = 1000;
	}

	/* Handle sleeping processes if any exist */

	if(!isempty(sleepq)) {

		/* Decrement the delay for the first process on the	*/
		/*   sleep queue, and awaken if the count reaches zero	*/

		if((--queuetab[firstid(sleepq)].qkey) <= 0) {
			wakeup();
		}
	}

	/* Decrement the preemption counter, and reschedule when the */
	/*   remaining time reaches zero			     */
	

	if(prptr->user_proc && prptr->prstate == PR_CURR) {
		prptr->prttl++;
		prptr->burst_duration++;
		if(prptr->prttl >= TIME_ALLOTMENT) {
			prptr->prttl = 0;
			if(prptr->prpriolvl != priolevel3){
				prptr->prpriolvl += 2;
				kprintf("prio level %d, pid %d\n", prptr->prpriolvl, currpid);
			}
		}
	}


	if((--time_period) <= 0) {
		time_period = PRIORITY_BOOST_PERIOD;
		for(i = 0; i < NPROC; i++) {
			proctab[i].prttl = 0;
			//proctab[i].prpriolvl = priolevel1;
		}
		while(nonempty(priolevel3)) {
			pid = dequeue(priolevel3);
			kprintf("timeperiod expire L3 pid: %d\n", pid);
			insert(pid, priolevel1, proctab[pid].prprio);
		}
		while(nonempty(priolevel2)) {
			pid = dequeue(priolevel2);
			kprintf("timeperiod expire L2 pid: %d\n", pid);
			insert(pid, priolevel1, proctab[pid].prprio);
		}
	}

	if((--preempt) <= 0) {
		preempt = TIME_SLICE;
		resched();
	}
}
