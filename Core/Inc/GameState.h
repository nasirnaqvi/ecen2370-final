/*
 * GameState.h
 *
 *  Created on: Apr 17, 2025
 *      Author: nana3231exc
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifndef INC_GAMESTATE_H_
#define INC_GAMESTATE_H_

/* Player macros */
#define EMPTY 0
#define PLAYER_ONE 1
#define PLAYER_TWO 2

#define ONE_PLAYER_MODE PLAYER_ONE
#define TWO_PLAYER_MODE PLAYER_TWO

#define PLAYER_ONE_TURN PLAYER_ONE
#define PLAYER_TWO_TURN PLAYER_TWO


#define AI_PLAYER    PLAYER_TWO_TURN
#define HUMAN_PLAYER PLAYER_ONE_TURN


#define NUM_ROWS 6
#define NUM_COLS 7
#define START_COL 4


typedef struct {
	uint8_t grid[6][7];
	uint8_t turn;
	bool playing;
	bool allowPlacement;
	uint32_t player1wins;
	uint32_t player2wins;
	uint32_t playTime;
	uint32_t playerMode;
} GameStructure_t;

extern GameStructure_t game;


void createNewGame();
void initializeGame();
bool checkForWin(uint8_t player);
bool isBoardFull();
int getAvailableRow(uint8_t col);
int dropPuck(uint8_t col, uint8_t player);
bool isValidMove(uint8_t col);

#endif /* INC_GAMESTATE_H_ */
