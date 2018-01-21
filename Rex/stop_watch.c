/*
 Stop Watch Library
 * Author: Shawn Standfast
 * Date: 12/8/2017
 * 
 * Requires Timer0 to be configured as a 1ms timer
 */

#include "stop_watch.h"


void StopWatch_Initialize (void)
{
    TMR0_StopTimer();
    
    globalTime = 0;
    
    TMR0_Initialize();
    
    TMR0_SetInterruptHandler(StopWatchISR);
    
   // TMR0_StartTimer();
    
}

void StartStopWatch (void)
{
    TMR0_StartTimer();
}
uint32_t GetCurrentTime (void)
{
    return globalTime;
}

void StopWatchISR (void)
{
    globalTime++;
}
