#include <linux/module.h>
#include <linux/init.h>

#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_sched.h>

#define ARG 0
#define STACK_SIZE 1024
#define PRIORITY RT_SCHED_HIGHEST_PRIORITY
#define USE_FPU 1
#define NOW rt_get_time()
#define PERIOD nano2count(10e9)

/* Store data needed for the thread */
RT_TASK thread_data;
RT_TASK thread_data1;
RT_TASK thread_data2;
/* The code that is run */

void thread_code(long arg)
{
        rt_task_suspend(&thread_data2);
	printk("this is grandpa!\n");
	rt_task_resume(&thread_data1);
        rt_task_wait_period();
}

void thread_code1(long arg)
{
        
	printk("this is father!\n");
	rt_task_resume(&thread_data2);
}

void thread_code2(long arg)
{
	rt_task_suspend(&thread_data1);
	printk("this is son!\n");
        rt_task_resume(&thread_data);
}

/* Called when "insmod" is used */
static int __init template_init(void)
{
  /* Start the RT timer, NB only needs to be done once */
  rt_set_periodic_mode();
  start_rt_timer(PERIOD);

  /* Initialise the data associated with a thread and make it periodic */
  rt_task_init(&thread_data, thread_code,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&thread_data, NOW, PERIOD);

  rt_task_init(&thread_data1, thread_code1,
	  ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);

  rt_task_init(&thread_data2, thread_code2,
	  ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  /* Return success */
  return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
  rt_task_delete(&thread_data);
  rt_task_delete(&thread_data1);
  rt_task_delete(&thread_data2);
}

module_init(template_init);
module_exit(template_exit);
