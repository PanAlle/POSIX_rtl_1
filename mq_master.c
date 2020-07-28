#include <signal.h>
#include <time.h>
#include <fcntl.h>    /* For O_* constants */
#include <sys/stat.h> /* For mode constants */
#include <mqueue.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define MY_MQ "/my_mq"
#define MY_MQ1 "/my_mq_1"

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
/***************************/

/* ****** MASTER TASK *******/

int main(void)
{
    mqd_t queue;
    struct mq_attr queue_attr;
	mqd_t queue1;
	//memset(&queue_attr1, 0, sizeof(queue_attr1));
    // Open the message queue read-only and non-blocking.
    queue_attr.mq_maxmsg = 10;
    queue_attr.mq_msgsize = sizeof(int);
    // Setup scheduler
    struct sched_param spar;
    spar.sched_priority = sched_get_priority_max(SCHED_FIFO);
    sched_setscheduler(0, SCHED_FIFO, &spar);
    // Setup timer
    init_timer(20 * 1000000);
	queue1 = mq_open(MY_MQ, O_WRONLY);
	if (queue1 <= 0) printf("queue1 creation failed, ERRNO: %d\n",errno);
	queue = mq_open(MY_MQ, O_RDONLY | O_CREAT | O_NONBLOCK, 0600, &queue_attr);
	if (queue <= 0) printf("queue creation failed, ERRNO: %d\n",errno);
    while (1)

    {	
	int mesg;	
	mq_receive(queue, (char *)&mesg, sizeof(mesg), NULL);
	printf("received pid %i\n", mesg);
	int answer = 2*mesg;
	if (mq_send(queue1, (char *)&answer, sizeof(int), spar.sched_priority) <= 0) printf("ERRNO: %d\n",errno);
	printf("--- New period ---\n");
	printf("------------------\n");
	sigwaitinfo(&signal_set, NULL);
    }

    return EXIT_SUCCESS;
}


