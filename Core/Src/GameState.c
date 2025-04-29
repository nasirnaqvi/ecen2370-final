/*
 * GameState.c
 *
 *  Created on: Apr 17, 2025
 *      Author: nana3231exc
 */

#include "GameState.h"

GameStructure_t game;

void createNewGame(void) {
    memset(game.grid, EMPTY, sizeof(game.grid));
    game.turn = PLAYER_ONE_TURN;
    game.playing = true;
    game.allowPlacement = true;
    game.playTime = 0;
}

void initializeGame(void){
    memset(game.grid, EMPTY, sizeof(game.grid));
    game.turn = PLAYER_ONE_TURN;
    game.playing = true;
    game.allowPlacement = true;
    game.player1wins = 0;
    game.player2wins = 0;
    game.playTime = 0;
}

bool checkForWin(uint8_t player) {
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            if (game.grid[row][col] != player) continue;

            // check horizontal
            if (col <= 3 &&
                game.grid[row][col + 1] == player &&
                game.grid[row][col + 2] == player &&
                game.grid[row][col + 3] == player) {
                return true;
            }

            // check vert
            if (row <= 2 &&
                game.grid[row + 1][col] == player &&
                game.grid[row + 2][col] == player &&
                game.grid[row + 3][col] == player) {
                return true;
            }

            // check diag right
            if (row <= 2 && col <= 3 &&
                game.grid[row + 1][col + 1] == player &&
                game.grid[row + 2][col + 2] == player &&
                game.grid[row + 3][col + 3] == player) {
                return true;
            }

            // check diag left
            if (row <= 2 && col >= 3 &&
                game.grid[row + 1][col - 1] == player &&
                game.grid[row + 2][col - 2] == player &&
                game.grid[row + 3][col - 3] == player) {
                return true;
            }
        }
    }

    return false;
}

bool isBoardFull() {
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            if (game.grid[row][col] == EMPTY) {
                return false;
            }
        }
    }
    return true;
}

bool isBoardFullWithBoard(uint8_t board[NUM_ROWS][NUM_COLS]) {
    for (int col = 0; col < NUM_COLS; col++) {
        if (board[NUM_ROWS - 1][col] == EMPTY) return false;
    }
    return true;
}

int getAvailableRow(uint8_t col){
    if (col >= NUM_COLS || col < 0) return -1;

    for (int row = 0; row < NUM_ROWS; row++) {
        if (game.grid[row][col] == EMPTY) {
            return row;
        }
    }
    return -1;
}


int dropPuck(uint8_t col, uint8_t player) {
	int row = getAvailableRow(col);
	if (row != -1) game.grid[row][col] = player;
	return row;
}

bool isValidMove(uint8_t col){
	return getAvailableRow(col) != -1;
}


