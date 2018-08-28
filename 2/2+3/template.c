#include <linux/module.h>
#include <linux/init.h>

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_shm.h>
#include <rtai_sched.h>
#include <rtai_nam2num.h>

#include <linux/comedi.h>
#include <linux/comedilib.h>

#define ARG 0
#define BUFFER 10
#define STACK_SIZE 1024
#define PRIORITY RT_SCHED_HIGHEST_PRIORITY
#define USE_FPU 1
#define NOW rt_get_time()
#define PERIOD nano2count(1e8)

#define READ_SUBDEVICE 0
#define WRITE_SUBDEVICE 1
#define READ_CHANNEL 0
#define WRITE_CHANNEL 0
#define RANGE 0
#define AREF AREF_GROUND

/* Store data needed for the thread */
RT_TASK thread_data;

/* Data needed by comedi */
comedi_t *comedi_dev;

int *buffer;
int in = 0, out = 0;
void write(int* value){ 
	buffer[in] = *value;
	in++; 
	if (in == BUFFER){
		in = 0;
	}
}
int read(){ 
	int result = buffer[out];
	out++;
	if (out == BUFFER){ 
		out = 0;
	}
	return result;
}
int temp;

int *mem1,*mem2;

int transfer(int x)
{
    int y;
    if (x-temp>500||temp-x>500)
	{
		y = 180;
	}
	else
	{
		y = (2047 - x) * 180;
		y = y / 2047;
		if (y<0) {
			y = y + 360;
		}
	}
    return y;
}

void share()
{
	*mem1 = in;
	*mem2 = out;
}

/* The code that is run */
void thread_code(long arg)
{
  while (1)
  {
    	int read_value, write_value = 0; /* What value should write_value be? */
        comedi_data_read(comedi_dev, READ_SUBDEVICE, READ_CHANNEL, RANGE, AREF, &read_value);
        int degree = transfer(read_value);
        printk("Data = %d\n", degree);
		temp = read_value;
        write(&degree);
		share();
    	comedi_data_write(comedi_dev, WRITE_SUBDEVICE, WRITE_CHANNEL, RANGE, AREF, write_value);
    /* Add code here */
    	rt_task_wait_period();
  }
}

/* Called when "insmod" is used */
static int __init template_init(void)
{
  /* Start the RT timer, NB only needs to be done once */
  rt_set_periodic_mode();
  start_rt_timer(PERIOD);

  comedi_dev = comedi_open("/dev/comedi0");

  buffer = rtai_kmalloc(nam2num("shmem0"), sizeof(int)*10);
  mem1 = rtai_kmalloc(nam2num("shmem1"), sizeof(int));
  mem2 = rtai_kmalloc(nam2num("shmem2"), sizeof(int));
  
  /* Initialise the data associated with a thread and make it periodic */
  rt_task_init(&thread_data, thread_code, ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&thread_data, NOW, PERIOD);

  /* Return success */
  return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
  rt_task_delete(&thread_data);
  
  rtai_kfree(nam2num("shmem0"));
  rtai_kfree(nam2num("shmem1"));
  rtai_kfree(nam2num("shmem2"));
  
  comedi_close(comedi_dev);
}

module_init(template_init);
module_exit(template_exit);
