#include<stdio.h>


int main(void)
{
	fork();
	while(1)
	{
		printf("hello world\n");
		sleep(1);
	}
	return 0;
}
