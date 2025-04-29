/*
 * Scheduler.h
 *
 *  Created on: Apr 18, 2025
 *      Author: nana3231exc
 */
#include <stdint.h>


#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_


//#define LED_TOGGLE_EVENT 	(1 << 0)
//#define DELAY_EVENT 		(1 << 1)
//
//#define BUTTON_EVENT		(1 << 2)

#define ONE_PLAYER_GAME 	(1 << 0)
#define TWO_PLAYER_GAME 	(1 << 1)


uint32_t getScheduledEvents();
void addSchedulerEvents(uint32_t event);
void removeSchedulerEvent(uint32_t event);
void clearSchedulerEvents();


#endif /* INC_SCHEDULER_H_ */
