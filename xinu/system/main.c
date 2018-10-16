#include <xinu.h>
#include <stdio.h>

//TIME_SLICE            2
//TIME_ALLOTMENT        10
//PRIORITY_BOOST_PERIOD 300

int main() {
	pid32 prA, prB;
	int i;

    prA = create_user_proc(burst_execution, 1024, 40, "burst_execution", 3, 1, 40, 0);
    prB = create_user_proc(burst_execution, 1024, 36, "burst_execution", 3, 2, 18, 6);
    resume(prA);
    resume(prB);
	sleepms(100);
	for(i=0;i<1000000;i++);
    
    return OK;
}


