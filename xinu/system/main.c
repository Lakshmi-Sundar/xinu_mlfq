#include <xinu.h>
#include <stdio.h>

//TIME_SLICE            2
//TIME_ALLOTMENT        10
//PRIORITY_BOOST_PERIOD 70


int main() {
	pid32 prA, prB, prC;
	int i;

    prA = create_user_proc(burst_execution, 1024, 36, "burst_execution", 3, 1, 36, 0);
    resume(prA);
    sleepms(30);
	//for(i=0;i<1000000;i++);
    prB = create_user_proc(burst_execution, 1024, 38, "burst_execution", 3, 1, 38, 0);
    prC = create_user_proc(burst_execution, 1024, 28, "burst_execution", 3, 2, 14, 4);
    resume(prB);
	resume(prC);
    sleepms(100);
	//for(i=0;i<1000000;i++);

    return OK;
}


