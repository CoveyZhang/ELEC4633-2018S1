#include <linux/module.h>
#include <linux/init.h>

#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>
#include <rtai_sched.h>

#define ARG 0
#define STACK_SIZE 1024
#define PRIORITY RT_SCHED_HIGHEST_PRIORITY
#define USE_FPU 1
#define NOW rt_get_time()
#define PERIOD nano2count(3e9)

int *mem;

/* Store data needed for the thread */
RT_TASK thread_data;
/* The code that is run */

void thread_code(long arg)
{
  int con=0;
  *mem = con;

  while (1)
  {
    /* Add code here */
	con++;
    *mem = con;
    rt_task_wait_period();
  }
}

/* Called when "insmod" is used */
static int __init template_init(void)
{
  /* Start the RT timer, NB only needs to be done once */
  rt_set_periodic_mode();
  start_rt_timer(PERIOD);

  mem = rtai_kmalloc(nam2num("shmem"), sizeof(int));

  /* Initialise the data associated with a thread and make it periodic */
  rt_task_init(&thread_data, thread_code,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&thread_data, NOW, PERIOD);

  /* Return success */
  return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
  rt_task_delete(&thread_data);
  rtai_kfree(nam2num("shmem"));
}

module_init(template_init);
module_exit(template_exit);
