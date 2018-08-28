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

int temp = 4095;

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
		y = (x-2047) * 180;
		y = y / 2047;
		if (y<0) {
			y = y + 360;
		}
	}
    return y;
}

int trans(int x)
{
    int y;
	if(x>=0&&x<=170) 
	{
		y=x+10*x/180;
	}
	if(x>=190&&x<=360)
	{
		y=x-10*(360-x)/180;
	}
    return y;
}


/* The code that is run */
void thread_code(long arg)
{
  while (1)
  {
    	int read_value, write_value = 0; /* What value should write_value be? */
        comedi_data_read(comedi_dev, READ_SUBDEVICE, READ_CHANNEL, RANGE, AREF, &read_value);
        int degree = transfer(read_value);
        //printk("Data = %d\n", degree);
	temp = read_value;
        write(&degree);
	*mem1 = in;
	int set;
	set=*mem2;
        printk("%d\n",set);
	set=trans(set);
        int write1;
	write1= degree-set;
        write_value=write1*4095/360;
        comedi_data_write(comedi_dev, WRITE_SUBDEVICE, WRITE_CHANNEL, RANGE, AREF, 0.7*write_value+2047);
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
