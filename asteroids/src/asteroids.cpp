


/* C libraries */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

/* hardware platform libraries */
#include <mbed.h>

#include "asteroids.h"
#include "controller.h"
#include "gamestate.h"
#include "draw.h"

Ticker gamestate, view, controller; //three tickers that handle the three main areas of the program


int score = 0;
int lives = 5;

bool menu = true;
bool lifeLost = false;
bool endGame = false;


int main() //main function of program
{
	
	
	
	initialise_heapM(); //initalises the heap for storing missile instances
	initialise_heapA(); //initalises the heap for storing asteroid instances
	init_double_buffering(); //initalises double buffering for smooth animation
	initGamestate(); //initalises game parameters to their default values, such as position of ship
	
	gamestate.attach(physics, 0.01); //attaches the function void physics(void) to the model ticker, this is the main function that calls other functions relating to the physics of the game
	view.attach(draw, 1.0/50.0); //same as above but for drawing onto the screen of the board, void draw(void) is the main drawing function //originally (draw, 1.0/50.0)
	controller.attach(userinput, 0.01); //same as above but for determining user input, void userinput(void) is the main function for capturing user input

}


//getter functions
int getScore(void)
{
	return score;
} 

int getLives(void)
{
	return lives;
}

bool getMenuState(void)
{
	return menu;
}

bool getLifeLost(void)
{
	return lifeLost;
}

bool getEndGame(void)
{
	return endGame;
}








//simple mutator functions for controlling various aspects of the game from other modules
void incScore(void)
{
	score++;
}

void decLives(void)
{
	lives--;
	setLifeLostTrue();
}

void setMenuTrue(void)
{
	menu = true;
}

void setMenuFalse(void)
{
	menu = false;
}
 
void setLifeLostTrue(void)
{
	lifeLost = true;
}

void setLifeLostFalse(void)
{
	lifeLost = false;
}


void setEndGameTrue(void)
{
	endGame = true;
}

void setEndGameFalse(void)
{
	endGame = false;
}


void restartGame(void)
{
	lives = 5;
	score = 0;
	initGamestate();
}
