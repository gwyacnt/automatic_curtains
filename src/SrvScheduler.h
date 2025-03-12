#ifndef __SRV_SCHEDULER_H
#define __SRV_SCHEDULER_H

#include <zephyr/kernel.h>

#define MAX_TASKS 5  // Adjust based on system needs

typedef void (*TaskFunction)(void);

typedef struct 
{
    TaskFunction task;
    uint32_t interval_ms;
    uint32_t last_run;
} ScheduledTask;

void SrvScheduler_Init(void);
void SrvScheduler_AddTask(TaskFunction task, uint32_t interval_ms);
void SrvScheduler_Start(void);

#endif // __SRV_SCHEDULER_H
