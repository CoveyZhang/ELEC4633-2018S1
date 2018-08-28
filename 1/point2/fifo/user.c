#include <stdio.h>

#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>

int main(void)
{
    int con;
    int fifo;
    
    fifo=open("/dev/rtf0",O_RDONLY);
  /* Add code */
  while (1)
  {
      read(fifo,&con,sizeof(con));
      printf("counter is %d\n",con);
  }
  return 0;
}

