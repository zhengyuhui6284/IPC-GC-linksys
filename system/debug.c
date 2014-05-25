/* debug.c */

#include <xinu.h>
#include <stdio.h>
#include <debug.h>

static void hexdump_print(byte, byte);

/**
 * Print a byte of data in mode.
 * @param data   byte of data to print
 * @param mode   ASCII or HEX
 */
static void hexdump_print(byte data, byte mode)
{
    switch (mode)
    {
    case DEBUG_ASCII:
        data = (' ' <= data && data <= '~') ? data : '.';
        fprintf(stdout, "%c", data);
        break;
    case DEBUG_HEX:
        fprintf(stdout, "%02x ", data);
        break;
    default:
        break;
    }
}

/**
 * Dump a buffer of given length to the to a device.
 * @param *buffer buffer to print out data in
 * @param length  length of buffer to print
 * @param canon   canonical representation (hex+ASCII)
 */
void hexdump(void *buffer, uint32 length, bool8 canon)
{
    uint32 m, n, remain;

    byte *b = (byte *)buffer;

    for (n = 0; n < length; n += 0x10)
    {
        fprintf(stdout, "%08x ", (uint32)buffer + n);

        remain = length - n;

        for (m = 0; m < remain && m < 0x10; m++)
        {
            if (m % 0x08 == 0)
            {
                fprintf(stdout, " ");
            }
            hexdump_print(b[n + m], DEBUG_HEX);
        }

        /* pad the rest if needed */
        if (remain < 0x10)
        {
            for (m = 0; m < 0x10 - remain; m++)
            {
                if ((0 != m) && (0 == m % 0x08))
                {
                    fprintf(stdout, " ");
                }
                fprintf(stdout, "   ");
            }
        }

        if (TRUE == canon)
        {
            fprintf(stdout, " |");
            for (m = 0; m < remain && m < 0x10; m++)
            {
                hexdump_print(b[n + m], DEBUG_ASCII);
            }
            fprintf(stdout, "|");
        }

        fprintf(stdout, "\n");
    }
}

/*********************************************
 * Show readylist && current process 
 * ******************************************/
void show_readylist(void) {

	int16 node;
	struct procent *prptr;

	node = firstid(readylist);
	prptr = &proctab[node];

	if (proctab[currpid].prprio == 0) {
		kprintf("[show_readylist]: null is current process\r\n");
		return;
	}

	kprintf("[show_readylist]: %s(%d): ", proctab[currpid].prname, proctab[currpid].prprio);
	while (node != queuetail(readylist)) {
		kprintf("%s(%d) -> ", prptr->prname, prptr->prprio);
		node = queuetab[node].qnext;
		prptr = &proctab[node];
	}
	
	kprintf("tail\r\n");
	return;
}



/**********************************
 * Debug purpose
 * THIS FUNTION IS BUGGY!!!
 * ********************************/
void show_message(pid32 pid) {

	struct procent *prptr;
	prptr = &proctab[pid];

	kprintf("[show_message]: ");
	kprintf("state: %d, message#: %d, queue: ", prptr->prstate, prptr->prhasmsgb);
	kprintf(isbadqid(prptr->tobe_send)? "bad": "Yes");
	kprintf("\r\n");

}
