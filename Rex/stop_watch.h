/* 
 * File:   stop_watch.h
 * Author: Kelly
 *
 * Created on December 9, 2017, 12:27 AM
 */

#ifndef STOP_WATCH_H
#define	STOP_WATCH_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>
#include "mcc_generated_files/tmr0.h"
 
volatile uint32_t globalTime;

void StopWatchISR (void);

void StopWatch_Initialize (void);

void StartStopWatch (void);

uint32_t GetCurrentTime (void);

    

#ifdef	__cplusplus
}
#endif

#endif	/* STOP_WATCH_H */

