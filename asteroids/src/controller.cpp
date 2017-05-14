

//include statements
#include <mbed.h>
#include "controller.h"
#include "gamestate.h"
#include "asteroids.h"

bool canFire = true; //can create a missile
bool canAsteroid = true; //can spawn an asteroid

void userinput(void)
{
			testUserInput(); //function called by the controller ticker
}




void testUserInput(void) //function that handles the joystick input
{
	if(!right)
	{
		incHeading(); //if right is pressed down, rotate the ship right for as long as the joystick is held down
	}
	
	if(!left)
	{
		decHeading(); //if left is pressed down, rotate the ship left for as long as the joystick is held down
	}
	
	if(!up)
	{
		addThrust(); //apply thrust if up is held down
	}
	else
	{
		applyFriction(); //when up is released, apply friction to the ship's velocity, eventually slowing the ship to a halt
	}
	
	if(!center) //if center is pressed, handle the button press based on the current state of the game
	{
		if(canFire) //if canFire is true, create a new missile instance
		{
			newMissile();
			canFire = false; //set canFire to false to prevent constant stream of missiles
		}
		
		if(getMenuState() == true) //if the menu is enabled, allow the user to close the menu and continue the game using the center button
		{
			setMenuFalse();
		}
		
		if(getEndGame() == true) //if the game has ended, allow the user to restart the game using the center button
		{
			setEndGameFalse();
			restartGame(); //restart the game and initalise any relevant variables such as lives and score
		}
	}
	
	if(center) //if center is released, set canFire to true to allow the user to fire another missile
	{
		canFire = true;
		
		if(getMenuState() == true) //prevent firing of missiles is the menu is enabled
		{
			canFire = false;
		}
		
	}
	
	
}

