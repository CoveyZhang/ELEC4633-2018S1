#include <stdio.h>

#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>
#include <sys/msg.h>

#define BUFFER 10

int *in;

int *buffer;

int out = 0;

struct mymsgbuf {
	long mtype;
	int data[10];
};

struct mymsgbuf message;

int read1(void) 
{
	int result = buffer[out];
	out++;
	if (out == BUFFER) 
	{
		out = 0;
	}
	return result;
}

int main(void)
{
	buffer= rtai_malloc(nam2num("shmem0"), sizeof(int)*10);
	in = rtai_malloc(nam2num("shmem1"), sizeof(int));
        
    int i;
	int status;
	int queue;
	key_t msgkey;

	msgkey = ftok(".", 'm');

	queue = msgget(msgkey, IPC_CREAT | 0660);
	
	 message.mtype = 1;

	while (1)
	{
          if(*in==9)
		  {
                for(i=0;i<10;i++)
                {
                	//printf("%d",read1());
					message.data[i] = read1();
					status = msgsnd(queue, &message, sizeof(message.data),0);
                }

          }
	}
	rtai_free(nam2num("shmem0"), *buffer);
	rtai_free(nam2num("shmem1"), *in);
	
	return 0;
}
