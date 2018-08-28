#include <stdio.h>

#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>

#define BUFFER 10

int *in;

int *buffer;

int out = 0;

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
	//out = rtai_malloc(nam2num("shmem2"), sizeof(int));
        
        int i;

	while (1)
	{
            if(*in==9)
            {
                for(i=0;i<10;i++)
                {
                    printf("degree is %d\n", read1());
                }
            }
	}
	rtai_free(nam2num("shmem0"), *buffer);
	rtai_free(nam2num("shmem1"), *in);
	//rtai_free(nam2num("shmem2"), *mem2);
	

	return 0;
}
