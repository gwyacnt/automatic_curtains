#include "SrvScheduler.h"
#include "HalTim.h"

static ScheduledTask tasks[MAX_TASKS];
static uint8_t task_count = 0;
static uint32_t system_time = 0;

// Timer callback for scheduling
static void SrvScheduler_Run(void) 
{
    system_time += 10;  // Assume timer runs every 10ms

    for (uint8_t i = 0; i < task_count; i++) 
    {
        if ((system_time - tasks[i].last_run) >= tasks[i].interval_ms) 
        {
            tasks[i].last_run = system_time;
            tasks[i].task();  // Execute task
        }
    }
}

// Initialize scheduler
void SrvScheduler_Init(void) 
{
    task_count = 0;
    HalTimer_RegisterCallback(SrvScheduler_Run);
    HalTimer_Init();
}

// Add a new task to the scheduler
void SrvScheduler_AddTask(TaskFunction task, uint32_t interval_ms) 
{
    if (task_count < MAX_TASKS) 
    {
        tasks[task_count].task = task;
        tasks[task_count].interval_ms = interval_ms;
        tasks[task_count].last_run = 0;
        task_count++;
    }
}

// Start scheduler
void SrvScheduler_Start(void) 
{
    HalTimer_Start(10);
}
