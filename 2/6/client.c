#include <stdio.h>
#include <sys/msg.h>

struct mymsgbuf {
	long mtype;
	int data[1];
};

struct mymsgbuf message;

int main(void)
{

	int status;
	int queue;
	key_t msgkey;

	msgkey = ftok(".", 'a');

	queue = msgget(msgkey, IPC_CREAT | 0660);

	
	message.mtype = 1;
        
	while (1)
	{
		scanf("%d", &message.data[0]);
		printf("setpoint:%d\n", message.data[0]);

		status = msgsnd(queue, &message, sizeof(message.data), 0);

	}

	return 0;
}
