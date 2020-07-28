#include <signal.h>
#include <time.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <mqueue.h>
#include <sched.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MY_MQ "/my_mq"
#define SLAVE_PRIORITY 1
#define SLAVE_PERIOD 10

sigset_t signal_set;

void init_timer(long period)
{
    timer_t timer;
    struct itimerspec timer_spec;

    // Create a timer.
    timer_create(CLOCK_REALTIME, NULL, &timer);

    // Specify the timer’s period.
    timer_spec.it_value.tv_sec = 0;
    timer_spec.it_value.tv_nsec = 1; /* Start immediately */
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = period;

    // Setup a signal set for sigwait() to wait for SIGALRM
    sigemptyset(&signal_set);
    sigaddset(&signal_set, SIGALRM);
    sigprocmask(SIG_BLOCK, &signal_set, NULL);

    // Setup the timer’s period.
    timer_settime(timer, 0, &timer_spec, NULL);
}
/* ****** SLAVE TASK *******/
int main(void)
{
    mqd_t queue;
    queue = mq_open(MY_MQ, O_WRONLY, 0644);
    // Setup scheduler
    struct sched_param spar;
    spar.sched_priority = sched_get_priority_min(SCHED_FIFO) + SLAVE_PRIORITY;
    if(sched_setscheduler(0, SCHED_FIFO, &spar) < 0)
    {
        printf("Unable to set SCHED_FIFO. %d\n", errno);
        exit(EXIT_FAILURE);
    }
    // Setup timer
    init_timer(SLAVE_PERIOD * 1000000);
    int mesg = (int)getpid();
    while (1)
    {
	sigwaitinfo(&signal_set, NULL);
	if (mq_send(queue, (char *)&mesg, sizeof(mesg), SLAVE_PRIORITY) < 0)
	{
	    printf("Unable to send. %d\n", errno);
	}
    }

    return EXIT_SUCCESS;
}
/**** END - SLAVE TASK *** */
