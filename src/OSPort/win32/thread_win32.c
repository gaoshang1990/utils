#include <windows.h>

#include "hal_thread.h"

struct sThread {
    ThreadExecutionFunction function;
    void*                   parameter;
    HANDLE                  handle;
    int                     state;
    bool                    autodestroy;
};

static DWORD WINAPI destroyAutomaticThreadRunner(LPVOID parameter);
static DWORD WINAPI threadRunner(LPVOID parameter);

static DWORD WINAPI destroyAutomaticThreadRunner(LPVOID parameter)
{
    Thread thread = (Thread)parameter;

    thread->function(thread->parameter);

    thread->state = 0;

    Thread_destroy_(thread);

    return 0;
}

static DWORD WINAPI threadRunner(LPVOID parameter)
{
    Thread thread = (Thread)parameter;

    return (UINT)thread->function(thread->parameter);
}

Thread Thread_create_(ThreadExecutionFunction function, void* parameter, bool autodestroy)
{
    DWORD  threadId;
    Thread thread = (Thread)malloc(sizeof(struct sThread));

    thread->parameter   = parameter;
    thread->function    = function;
    thread->state       = 0;
    thread->autodestroy = autodestroy;

    if (autodestroy == true)
        thread->handle = CreateThread(
            0, 0, destroyAutomaticThreadRunner, thread, CREATE_SUSPENDED, &threadId);
    else
        thread->handle =
            CreateThread(0, 0, threadRunner, thread, CREATE_SUSPENDED, &threadId);

    return thread;
}

void Thread_start_(Thread thread)
{
    thread->state = 1;
    ResumeThread(thread->handle);
}

void Thread_destroy_(Thread thread)
{
    if (thread->state == 1)
        WaitForSingleObject(thread->handle, INFINITE);

    CloseHandle(thread->handle);

    free(thread);
}

void Thread_sleep(int millies)
{
    Sleep(millies);
}

Semaphore Semaphore_create(int initialValue)
{
    HANDLE self = CreateSemaphore(NULL, initialValue, 1, NULL);

    return self;
}

/* Wait until semaphore value is greater than zero. Then decrease the semaphore value. */
void Semaphore_wait(Semaphore self)
{
    WaitForSingleObject((HANDLE)self, INFINITE);
}
int Semaphore_wait_timeout(Semaphore self, int ms)
{
    DWORD dw = WaitForSingleObject((HANDLE)self, ms);
    switch (dw) {
    case WAIT_OBJECT_0:
        return 0;
    case WAIT_TIMEOUT:
    case WAIT_FAILED:
    default:
        return -1;
    }
}

void Semaphore_post(Semaphore self)
{
    ReleaseSemaphore((HANDLE)self, 1, NULL);
}

void Semaphore_destroy(Semaphore self)
{
    CloseHandle((HANDLE)self);
}

uint32_t Thread_getTID(void)
{
    return (uint32_t)GetCurrentThreadId();
}

uint32_t Thread_getPID(void)
{
    return (uint32_t)GetCurrentProcessId();
}
