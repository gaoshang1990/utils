#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h>

#include "hal_thread.h"

struct sThread {
    ThreadExecutionFunction function;
    void*                   parameter;
    pthread_t               pthread;
    int                     state;
    bool                    autodestroy;
};

Semaphore Semaphore_create(int initialValue)
{
    Semaphore self = malloc(sizeof(sem_t));

    sem_init((sem_t*)self, 0, initialValue);

    return self;
}

/* Wait until semaphore value is more than zero. Then decrease the semaphore value. */
void Semaphore_wait(Semaphore self)
{
    sem_wait((sem_t*)self);
}

int Semaphore_wait_timeout(Semaphore self, int ms)
{
#define NSECTOSEC 1000000000
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) < 0)
    {
        // printf("clock_gettime ..............failed! \n");
        return -1;
    }
    //	printf("[0] tv.sec=%ld ,tv.nsec=%ld timeout=%d ms\n",ts.tv_sec,ts.tv_nsec,ms);
    ts.tv_sec += ms / 1000;
    ts.tv_nsec += (ms % 1000) * 1000000;
    ts.tv_sec += ts.tv_nsec / NSECTOSEC;
    ts.tv_nsec = ts.tv_nsec % NSECTOSEC;
    //	printf("[1] tv.sec=%ld ,tv.nsec=%ld \n",ts.tv_sec,ts.tv_nsec);
    if (sem_timedwait((sem_t*)self, &ts) == -1)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
void Semaphore_post(Semaphore self)
{
    sem_post((sem_t*)self);
}

void Semaphore_destroy(Semaphore self)
{
    sem_destroy((sem_t*)self);
    free(self);
}

/* xu 函数名与mqtt库冲突，故修改 */
Thread Thread_create_(ThreadExecutionFunction function, void* parameter, bool autodestroy)
{
    Thread thread = (Thread)malloc(sizeof(struct sThread));

    if (thread != NULL)
    {
        thread->parameter   = parameter;
        thread->function    = function;
        thread->state       = 0;
        thread->autodestroy = autodestroy;
    }

    return thread;
}

static void* destroyAutomaticThread(void* parameter)
{
    Thread thread = (Thread)parameter;

    thread->function(thread->parameter);

    free(thread);

    pthread_exit(NULL);
}

void Thread_start_(Thread thread)
{
    if (thread->autodestroy == true)
    {
        pthread_create(&thread->pthread, NULL, destroyAutomaticThread, thread);
        pthread_detach(thread->pthread);
    }
    else
        pthread_create(&thread->pthread, NULL, thread->function, thread->parameter);

    thread->state = 1;
}

void Thread_destroy_(Thread thread)
{
    if (thread->state == 1)
    {
        pthread_join(thread->pthread, NULL);
    }

    free(thread);
}

void Thread_sleep(int millies)
{
    usleep(millies * 1000);
}

uint32_t Thread_getTID(void)
{
	return (uint32_t)pthread_self();
}

uint32_t Thread_getPID(void)
{
    return (uint32_t)getpid();
}