

//Include statements 
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <display.h>

#include "draw.h"
#include "gamestate.h"
#include "asteroids.h"


Display *screen = Display::theDisplay(); //Display object for drawing on screen 
int timerValue = 0; //Debug




void draw(void) //main draw function called by the draw ticker
{ 
		
	
		if(getMenuState() == true) //if the menu is enabled, draw the menu
		{
			screen->fillScreen(BLACK);
			screen->setCursor(150,40);
			screen->setTextSize(3);
			screen->printf("asteroids");
			screen->setTextSize(1);
			screen->setCursor(5,5);
			screen->printf("Developed by Jake Mangan (W14014329) and Adam Newby (W14005199)");
			screen->setCursor(150,130);
			screen->printf("Press CENTER to start");
			
			
			if(getLifeLost() == true && getEndGame() == false) //if a life has been lost, but the game has not ended, tell the user they have lost a life and give the option to continue the game
			{
				screen->fillRect(150,130,200,20,BLACK);
				screen->setCursor(150,130);
				screen->printf("You have lost a life. Press CENTER to resume.");
				
			}
			
			if(getEndGame() == true) //if the game has ended, draw the end game screen
			{
				screen->fillRect(150,130,200,20,BLACK);
				screen->setCursor(150,130);
				screen->printf("All lives lost. Your score was %i.", getScore());
				screen->setCursor(150,150);
				screen->printf("Press CENTER to restart");
				
			}
			
		}
		else
		if(getMenuState() == false) //if the menu is not enabled, draw the active game state
		{
			screen->fillScreen(BLACK);
			//topLeftTimer(); //debug
			//showDebug(); //debug
			drawShip();
			drawMissiles(getMissileList());
			drawAsteroids(getAsteroidList());
			drawScoreboard();
		}
		
		swap_double_buffers();
		

	
}


void initScreen(void) 
{
	screen->fillScreen(WHITE);
  screen->setTextColor(BLACK, WHITE);
}




//Functions associated with double buffering, includes a function to initalise the two buffers and a function to swap between the two buffers when called

void init_double_buffering(void)
{
    uint16_t *buffer = screen->getFb(); 
    uint16_t *buffer2 = buffer+(480*272);
    LPC_LCD->UPBASE = (uint32_t)buffer2;
}

void swap_double_buffers(void)
{
    unsigned int b;
    b = LPC_LCD->UPBASE;
    LPC_LCD->UPBASE = (uint32_t)screen->getFb();
    screen->setFb((uint16_t *)b);
}


void drawShip(void) //pulls the current location of the ship from the ship struct and draws the ship based on its current location
{
	int shipX = getShipX();
	int shipY = getShipY();
	
	int shipX1 = getShipX1();
	int shipY1 = getShipY1();
	int shipX2 = getShipX2();
	int shipY2 = getShipY2();
	int shipX3 = getShipX3();
	int shipY3 = getShipY3();
	
	screen->drawPixel(shipX, shipY, MAGENTA);
	screen->drawTriangle(shipX1, shipY1, shipX2, shipY2, shipX3, shipY3, MAGENTA);
}


void drawShip3D(void) //draws the ship in a 3D fashion, aesthetic purposes only
{
	int shipX1 = getShipX1();
	int shipY1 = getShipY1();
	int shipX2 = getShipX2();
	int shipY2 = getShipY2();
	int shipX3 = getShipX3();
	int shipY3 = getShipY3();
	
	screen->drawLine(shipX1, (shipY1), shipX1, (shipY1 + 10), GREEN);
	screen->drawLine(shipX2, (shipY2), shipX2, (shipY2 + 10), GREEN);
	screen->drawLine(shipX3, (shipY3), shipX3, (shipY3 + 10), GREEN);
	
	screen->drawLine(shipX1, (shipY1 + 10), shipX2, (shipY2 + 10), GREEN);
	screen->drawLine(shipX2, (shipY2 + 10), shipX3, (shipY3 + 10), GREEN);
	screen->drawLine(shipX3, (shipY3 + 10), shipX1, (shipY1 + 10), GREEN);
}

void drawMissiles(struct particle *lst) //iterates through the list of active missiles that is passed through to the function, draws each active missile at its current location 
{
    
    while(lst) 
		{
        if(lst->draw == 1) //if the missile is to be drawn
				{
					screen->fillRect(lst->x, lst->y, 2, 2, MAGENTA); //draw at current location at size of 2
				}

        lst = lst->next; //iterate the list
    }
}

void drawAsteroids(struct particle *lst) //iterates through the list of active asteroids that is passed through to the function, draws each active asteroid at its current location 
{
	while(lst)
	{
		if(lst->draw == 1) //if the asteroid is to be drawn
		{
			screen->drawRect(lst->x, lst->y, lst->size, lst->size, RED); //drawn at current location and at size specified
		}
		lst = lst->next; //iterate the list
	}
}




void drawScoreboard(void) //Draws the scoreboard at the top of the screen displaying lives and score
{
	screen->fillRect(0,0,480,20, BLACK);
	screen->fillRect(0,20,480,3, WHITE);
	screen->setCursor(0,10);
	screen->printf("Lives: %i", getLives());
	screen->setCursor(420,10);
	screen->printf("Score: %i", getScore());
}






void topLeftTimer(void) //Print a constantly increasing integer for identifying when the program has frozen, for debugging 
{
		timerValue++;
		screen->setCursor(0,50);
		screen->printf("%i", timerValue);
}

void showDebug(void) //Prints a variety of debugging information onto the screen
{
	screen->setCursor(0,60);
	screen->printf("Ship x/y: (%f, %f)", getShipX(), getShipY());
	screen->setCursor(0,70);
	screen->printf("Ship vx/vy: (%f, %f)", getShipVx(), getShipVy());
	screen->setCursor(0,90);
	screen->printf("Collisions: %i", getCollisionCounter());
	screen->setCursor(0,100);
	screen->printf("Clusters: %i", getClusterCounter());
	screen->setCursor(0,110);
	screen->printf("AsteroidSpawnRate: %i", getAsteroidSpawnRate());
	screen->setCursor(0,120);
	screen->printf("incDifficulty: %i", getIncDifficulty());
}


