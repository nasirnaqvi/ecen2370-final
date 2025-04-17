/*
 * GameState.h
 *
 *  Created on: Apr 17, 2025
 *      Author: nana3231exc
 */

#include <stdint.h>
#include <stdbool.h>


#ifndef INC_GAMESTATE_H_
#define INC_GAMESTATE_H_

/* Player macros */
#define ONE_PLAYER_MODE 1
#define TWO_PLAYER_MODE 2

#define PLAYER_ONE_TURN 1
#define PLAYER_TWO_TURN 2

typedef struct {
	uint8_t grid[6][7];
	uint8_t playerMode;
	uint8_t turn;
	bool playing;
} GameStructure_t;

extern GameStructure_t game;

#endif /* INC_GAMESTATE_H_ */
