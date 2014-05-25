/* ***********************************
 * Testcase for Garbage Collection
 *
 * to use: overwrite main.c, recomplie
 * ***********************************/

#include <xinu.h>

uint32 recvbuf;		/* lab3-2: extern uint32 recvbuf */

/* allocate heap variables */
int memalloc(int unit) {

	int i;
	struct procent *prptr;
	prptr = &proctab[currpid];

	for (i=0; i<100; i++) {
		getmemgb(unit);	
	}
	
	intmask mask = disable();
	kprintf("[memory test]: %s(prname) allocated %10d\r\n", prptr->prname, prptr->prmemlist->mb_size);
	restore(mask);
	return 0;
}

/* free heap variables */
int memdealloc(int unit) {
	
	int i;
	struct procent *prptr;
	char *addr;
	prptr = &proctab[currpid];

	for (i=0; i<100; i++) {
		addr = getmem(unit);
		freemem(addr, unit);
	}

	intmask mask = disable();
	kprintf("[memory test]: %s(prname) used  %10d\r\n", prptr->prname, prptr->prmemlist->mb_size);
	restore(mask);
	return 0;
}

/* main */
int main(int argc, char** argv){

	kprintf("[memory test]: xinu has free memory %10d\r\n", memlist.mlength);

	resume(create(memdealloc, 2048, 20, "mem0", 1, 1));

	kprintf("[memory test]: xinu has free memory %10d\r\n", memlist.mlength);
	
	resume(create(memalloc, 2048, 20, "mem1", 1, 1));
	resume(create(memalloc, 2048, 20, "mem2", 1, 2));
	resume(create(memalloc, 2048, 20, "mem3", 1, 3));
	resume(create(memalloc, 2048, 20, "mem4", 1, 4));
	resume(create(memalloc, 2048, 20, "mem5", 1, 5));
	resume(create(memalloc, 2048, 20, "mem6", 1, 6));
	resume(create(memalloc, 2048, 20, "mem7", 1, 7));
	resume(create(memalloc, 2048, 20, "mem8", 1, 8));
	resume(create(memalloc, 2048, 20, "mem9", 1, 9));
	resume(create(memalloc, 2048, 20, "mem10", 1, 10));

	sleep(3);
	kprintf("[memory test]: xinu has free memory %10d\r\n", memlist.mlength);
	kprintf("\r\n");

	resume(create(memalloc, 2048, 20, "mem11", 1, 1));
	resume(create(memalloc, 2048, 20, "mem12", 1, 2));
	resume(create(memalloc, 2048, 20, "mem13", 1, 3));
	resume(create(memalloc, 2048, 20, "mem14", 1, 4));
	resume(create(memalloc, 2048, 20, "mem15", 1, 5));
	resume(create(memalloc, 2048, 20, "mem16", 1, 6));
	resume(create(memalloc, 2048, 20, "mem17", 1, 7));
	resume(create(memalloc, 2048, 20, "mem18", 1, 8));
	resume(create(memalloc, 2048, 20, "mem19", 1, 9));
	resume(create(memalloc, 2048, 20, "mem20", 1, 10));

	sleep(3);
	kprintf("[memory test]: xinu has free memory %10d\r\n", memlist.mlength);
	kprintf("\r\n");
	
	
	
	kprintf("[memory test]: main() finished\r\n");
	
	while (1) {
		sleep(1); 
	}

	return 0;
}

