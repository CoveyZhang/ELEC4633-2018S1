#include <stdio.h>
#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>
#include <sys/msg.h>

int *mem2;

struct mymsgbuf {
	long mtype;
	int data[1];
};

struct mymsgbuf setpoint;

int main(void)
{
	int status;
	int queue;
	key_t msgkey;

	mem2 = rtai_kmalloc(nam2num("shmem2"), sizeof(int));

	msgkey = ftok(".", 'a');

	queue = msgget(msgkey, IPC_CREAT | 0660);

        setpoint.mtype = 1;

	while (1)
	{
		status = msgrcv(queue, &setpoint, sizeof(setpoint.data), 0, 0);
                
		*mem2 = setpoint.data[0];
                printf("%d\n",*mem2);
	}
	rtai_free(nam2num("shmem2"), *mem2);

	return 0;
}
