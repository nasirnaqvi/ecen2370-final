/*
 * ApplicationCode.c
 *
 *  Created on: Dec 30, 2023 (updated 11/12/2024) Thanks Donavon! 
 *      Author: Xavion
 */

#include "ApplicationCode.h"

/* Static variables */
extern GameStructure_t game;
extern void initialise_monitor_handles(void); 

#if COMPILE_TOUCH_FUNCTIONS == 1
static STMPE811_TouchData StaticTouchData;
#endif // COMPILE_TOUCH_FUNCTIONS

void ApplicationInit(void)
{
//	initialise_monitor_handles(); // Allows printf functionality
    LTCD__Init();
    LTCD_Layer_Init(0);
    LCD_Clear(0,LCD_COLOR_WHITE);


    #if COMPILE_TOUCH_FUNCTIONS == 1
	InitializeLCDTouch();

	// This is the orientation for the board to be direclty up where the buttons are vertically above the screen
	// Top left would be low x value, high y value. Bottom right would be low x value, low y value.
	StaticTouchData.orientation = STMPE811_Orientation_Portrait_2;

	#endif // COMPILE_TOUCH_FUNCTIONS
}


void LCD_Visual_Demo(void)
{
//	visualDemo();
}

void startGame(void){
    LCD_Clear(0, LCD_COLOR_BLACK);
    LCD_printSelectModeScreen();


    while (1) {
        if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
            if (StaticTouchData.x > LCD_PIXEL_WIDTH / 2) {
                // Right side touched
                addSchedulerEvents(TWO_PLAYER_GAME);
                printf("Two Player Mode Selected\n");
                game.playerMode = ONE_PLAYER_MODE;
            } else {
                // Left side touched
                addSchedulerEvents(ONE_PLAYER_GAME);
                printf("One Player Mode Selected\n");
                game.playerMode = TWO_PLAYER_MODE;
            }
            break;
        }
    }
}

void playTwoPlayerGame(void) {
    game.playing = true;
    while (game.playing) {
        placingPuck();
		if (checkForWin(PLAYER_ONE)) break;
        placingPuck();
		if (checkForWin(PLAYER_TWO)) break;
    }
}


void playOnePlayerGame(void){
    game.playing = true;
    while (game.playing) {
		placingPuck();
		if (checkForWin(HUMAN_PLAYER)) break;
		AIMove();
		if (checkForWin(AI_PLAYER)) break;
	}
}



void AIMove(void) {
	uint8_t col, row;

	for (col = 0; col < NUM_COLS; col++) {
		row = getAvailableRow(col);
		if (row < 0) continue;
		dropPuck(col, AI_PLAYER);
		if (checkForWin(AI_PLAYER)) {
			goto finished;
		}
		game.grid[row][col] = EMPTY;
	}

	for (col = 0; col < NUM_COLS; col++) {
		row = getAvailableRow(col);
		if (row < 0) continue;
		game.grid[row][col] = HUMAN_PLAYER;
		if (checkForWin(HUMAN_PLAYER)) {
			game.grid[row][col] = EMPTY;
			dropPuck(col, AI_PLAYER);
			goto finished;
		}
		game.grid[row][col] = EMPTY;
	}

    for (int r = 0; r < NUM_ROWS; r++) {
        for (int c = 0; c < NUM_COLS - 1; c++) {
            if (game.grid[r][c] == HUMAN_PLAYER &&
                game.grid[r][c+1] == HUMAN_PLAYER) {

                if (c-1 >= 0 && game.grid[r][c-1] == EMPTY) {
                    int dropCol = c-1;
                    if (getAvailableRow(dropCol) == r) {
                        dropPuck(dropCol, AI_PLAYER);
                        goto finished;
                    }
                }
                if (c+2 < NUM_COLS && game.grid[r][c+2] == EMPTY) {
                    int dropCol = c+2;
                    if (getAvailableRow(dropCol) == r) {
                        dropPuck(dropCol, AI_PLAYER);
                        goto finished;
                    }
                }
            }
        }
    }


	if (isValidMove(NUM_COLS/2)) {
		dropPuck(NUM_COLS/2, AI_PLAYER);
		goto finished;
	}

	const uint8_t order[NUM_COLS] = {3,2,4,1,5,0,6};
	for (int i = 0; i < NUM_COLS; i++) {
		col = order[i];
		if (isValidMove(col)) {
			dropPuck(col, AI_PLAYER);
			goto finished;
		}
	}

	return;

finished:
	LCD_DrawBoard();

	if (checkForWin(AI_PLAYER)) {
		game.playing = false;
	} else {
		game.turn = HUMAN_PLAYER;
		game.allowPlacement = true;
	}
}






void placingPuck(void) {
    uint8_t col = START_COL;

    game.allowPlacement = true;
    LCD_DrawBoard();
    LCD_PlacePuck(col);

    uint8_t currentTurn = game.turn;

    while (currentTurn == game.turn) {
        if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
            if (StaticTouchData.x > LCD_PIXEL_WIDTH / 2) {
                if (col < NUM_COLS) col++;
            } else {
                if (col > 1) col--;
            }

            LCD_DrawBoard();
            LCD_PlacePuck(col);
            HAL_Delay(150);
        }

        if (game.turn != currentTurn) {
            int row = dropPuck(col-1, currentTurn);

            if (row != -1) {
                LCD_DrawBoard();

                if (checkForWin(currentTurn) || isBoardFull()) {
                    game.playing = false;
                    return;
                }

                game.turn = (currentTurn == PLAYER_ONE_TURN) ? PLAYER_TWO_TURN : PLAYER_ONE_TURN;
                game.allowPlacement = true;
                return;
            } else {
                game.turn = currentTurn;
                game.allowPlacement = true;
            }
        }
    }
}


void endGame(void) {
    LCD_Clear(0, LCD_COLOR_BLACK);
    LCD_SetFont(&Font16x24);
    LCD_SetTextColor(LCD_COLOR_WHITE);

	LCD_DrawBoard();
    LCD_DrawString(25, 25, "Game Over!");

	HAL_Delay(5000);





	char result[32];
	if (checkForWin(PLAYER_ONE_TURN)) {
	    strcpy(result, "Player 1 Wins!");
	    game.player1wins++;
	} else if (checkForWin(PLAYER_TWO_TURN)) {
	    strcpy(result, "Player 2 Wins!");
	    game.player2wins++;
	} else {
	    strcpy(result, "It's a Draw!");
	}

	LCD_printEndScreen(result);
    while (1) {
        if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
            if (StaticTouchData.x > LCD_PIXEL_WIDTH / 2) {
              	LCD_terminationScreen();
//            	LCD_DrawBoard();
              	while(1);
            } else {
                return;
            }
        }
    }
}


#if COMPILE_TOUCH_FUNCTIONS == 1
void LCD_Touch_Polling_Demo(void)
{
	LCD_Clear(0,LCD_COLOR_GREEN);
	while (1) {
		/* If touch pressed */
		if (returnTouchStateAndLocation(&StaticTouchData) == STMPE811_State_Pressed) {
			/* Touch valid */
			printf("\nX: %03d\nY: %03d\n", StaticTouchData.x, StaticTouchData.y);
			LCD_Clear(0, LCD_COLOR_RED);
		} else {
			/* Touch not pressed */
			printf("Not Pressed\n\n");
			LCD_Clear(0, LCD_COLOR_GREEN);
		}
	}
}

#endif // COMPILE_TOUCH_FUNCTIONS

void EXTI0_IRQHandler(void) {
    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
	EXTI_HandleTypeDef hexti;
	hexti.Line = EXTI_LINE_0;

	if (game.allowPlacement) {
		game.allowPlacement = false;
		game.turn = (game.turn == PLAYER_ONE_TURN) ? PLAYER_TWO_TURN : PLAYER_ONE_TURN;
	}


    HAL_EXTI_ClearPending(&hexti, EXTI_TRIGGER_RISING_FALLING);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}
//void EXTI0_IRQHandler(void) {
//    HAL_NVIC_DisableIRQ(EXTI0_IRQn);
//	EXTI_HandleTypeDef hexti;
//	hexti.Line = EXTI_LINE_0;
////    if (game.playerMode == TWO_PLAYER_MODE || (game.playerMode == ONE_PLAYER_MODE && game.turn == HUMAN_PLAYER)){
//		if (!game.allowPlacement) {
//			HAL_EXTI_ClearPending(&hexti, EXTI_TRIGGER_RISING_FALLING);
//			HAL_NVIC_EnableIRQ(EXTI0_IRQn);
//			return;
//		}
//
//		game.allowPlacement = false;
//		game.turn = (game.turn == PLAYER_ONE_TURN) ? PLAYER_TWO_TURN : PLAYER_ONE_TURN;
////    }
//
//    HAL_EXTI_ClearPending(&hexti, EXTI_TRIGGER_RISING_FALLING);
//    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
//}



