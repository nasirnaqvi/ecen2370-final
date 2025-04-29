/*
 * Scheduler.c
 *
 *  Created on: Apr 18, 2025
 *      Author: nana3231exc
 */

#include "Scheduler.h"

static uint32_t scheduledEvents = 0;


uint32_t getScheduledEvents(){
	return scheduledEvents;
}

void addSchedulerEvents(uint32_t event){
	scheduledEvents |= event;
}

void removeSchedulerEvent(uint32_t event){
	scheduledEvents	&= ~event;
}
