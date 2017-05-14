
//include statements
#include <stdlib.h>
#include <time.h>
#include "gamestate.h"
#include "asteroids.h"
#include "draw.h"
#include <mbed.h>

/*
*		variable delcaration
*/

//missile orientated variables
static const int heapsizeM = 20; //maximum number of active missiles 
static node_t heapM[heapsizeM];
static node_t *freenodesM;
struct particle *activeMissiles = 0; //missile linked list

//asteroid orientated variables
static const int heapsizeA = 30; //maximum number of active asteroids
static node_t heapA[heapsizeA];
static node_t *freenodesA;
struct particle *activeAsteroids = 0; //asteroid linked list


struct ship_t{ //ship structure, holds the variables that store the information regarding the ships coordinates/direction and others
	float heading;
	float x;
	float y;
	float x1, x2, x3;
	float y1, y2, y3;
	
	float vx, vy;
	float thrust;
} ship;

int asteroidTickCount = 0; //integer used as a timer to create new asteroids 
int clusterCounter = 0; //debug - total number of asteroids split into clusters
int collisionCounter = 0; //debug - total number of missile/asteroid collisions
int asteroidSpawnRate = 100; //lower is faster spawn rate
int incDifficulty = 0;


void physics(void) //main function called by gamestate ticker, contains the functions concerned with game physics
{
	
	if(getMenuState() == false) //if menu is not enabled
	{
		updateShipBorder(); 
		updateShipLocation();
		rotateShip();
		wrap(activeMissiles, activeAsteroids);
		updateMissiles(activeMissiles);
		updateAsteroids(activeAsteroids);
		checkMissileAsteroidCollision(activeAsteroids, activeMissiles); //check for missile/asteroid collisions
		checkShipAsteroidCollision(activeAsteroids); //test for ship/asteroid collisions
		
		asteroidTick(); //increase  asteroid timer
	
	}
	
	if(getLives() == 0) //if lives == 0, end the game
	{
		setEndGameTrue();
		setMenuTrue();
	}
}





void initGamestate(void) //called at the start of the game, initalises multiple variables for the game to run properly
{
	ship.x = 240;
	ship.y = 135;
	ship.heading = 0;
	ship.thrust = 0;
}

void updateShipBorder(void) //updates the ship's x1, x2, x3, y1, y2, y3 coordinates based on the ship's origin so that the ship can be drawn on screen in the form of a triangle
{
	ship.x1 = ship.x;
	ship.y1 = ship.y - 20;
	ship.x2 = ship.x - 10;
	ship.y2 = ship.y + 10;
	ship.x3 = ship.x + 10;
	ship.y3 = ship.y + 10;
}

void updateShipLocation(void) 
{
	ship.x = ship.x + (ship.vx / 2);
	ship.y = ship.y + (ship.vy / 2);
}

void incHeading(void) 
{
	ship.heading++;
	ship.heading++;
}

void decHeading(void)
{
	ship.heading--;
	ship.heading--;
}

void rotateShip(void) //uses the ship's heading variable to rotate and adjust the direction of the ship
{
	int x1 = ship.x;
	int y1 = ship.y + 20;
	int x2 = ship.x - 10;
	int y2 = ship.y - 10;
	int x3 = ship.x + 10;
	int y3 = ship.y - 10;
	float rads = getHeadingRads(); //returns the ship's heading in rads for use with cos() and sin() functions
	
	int x1r, x2r, x3r;
	int y1r, y2r, y3r;

	//below equations are based on rotation matrix theory
	x1r = ((x1 - ship.x) * cos(rads)) - ((ship.y - y1) * -(sin(rads))) + ship.x;
	y1r = ((ship.y - y1) * cos(rads)) - ((x1 - ship.x) * sin(rads)) + ship.y;
	x2r = ((x2 - ship.x) * cos(rads)) - ((ship.y - y2) * -(sin(rads))) + ship.x;
	y2r = ((ship.y - y2) * cos(rads)) - ((x2 - ship.x) * sin(rads)) + ship.y;
	x3r = ((x3 - ship.x) * cos(rads)) - ((ship.y - y3) * -(sin(rads))) + ship.x;
	y3r = ((ship.y - y3) * cos(rads)) - ((x3 - ship.x) * sin(rads)) + ship.y;
	
	ship.x1 = x1r;
	ship.y1 = y1r;
	ship.x2 = x2r;
	ship.y2 = y2r;
	ship.x3 = x3r;
	ship.y3 = y3r;
}

void addThrust() //adds x/y directional thrust using the ship's heading 
{
		float rads = getHeadingRads();
		
		if(ship.vx < thrustMax && ship.vx > -(thrustMax))
		{
			ship.vx += thrustInc * cos(rads + 1.5708);
		}
		
		if(ship.vy < thrustMax && ship.vy > -(thrustMax))
		{
			ship.vy += thrustInc * -(sin(rads + 1.5708));
		}
}

void applyFriction() //reduces the ships speed every tick whilst the up button isn't held 
{
		ship.vx *= friction;
		ship.vy *= friction;
}



//missile functions
float calcMissileVx(void) //calculates a missile's X velocity based on the ship's heading
{
	float rads = getHeadingRads();
	return 3 * cos(rads + 1.5708);
}

float calcMissileVy(void) //calculates a missile's Y velocity based on the ship's heading
{
	float rads = getHeadingRads();
	return 3 * -(sin(rads + 1.5708));
}


//asteroid functions

float calcAstRandX(void) //calculates a random spawn location for an asteroid along the X axis, the function will continue to calculate a random spawn if the chosen spawn is inside of the screen dimensions
{
	bool coordOkay = false;
	float randX;
	srand(time(0));
	
	while(!coordOkay)
	{
		randX = rand()%(530-(-50) + 1) + (-50);
		
		if(randX < -20 || randX > 500)
		{
			coordOkay = true;
		}
	}
	
	return randX;
}

float calcAstRandY(void) //calculates a random spawn location for an asteroid based along the Y axis
{
	srand(time(0));
	float randY = rand()%(240-60 + 1) + 60;
	return randY;
}

float calcAstRandVx(float heading) //calculates a random X velocity for an asteroid using the asteroid's heading 
{
	srand(time(0));
	float randSpeed = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //random inclusive float between 0.0 and 1
	
	if(randSpeed < 0.3) //increases the lower value of randSpeed to stop really slow asteroids
	{
		randSpeed = randSpeed + 1.3;
	}
	
	float rads = toRads(heading);
	return randSpeed * cos(rads + 1.5708);
	
}

float calcAstRandVy(float heading) //calculates a random Y velocity for an asteroid using the asteroid's heading 
{
	srand(time(0));
	float randSpeed = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); //random inclusive float between 0.0 and 1
	
	if(randSpeed < 0.3) //increases the lower value of randSpeed to stop really slow asteroids
	{
		randSpeed = randSpeed + 0.3;
	}
	
	float rads = toRads(heading);
	return randSpeed * -(sin(rads + 1.5708));
}


float calcAstSizeVx(float heading, int size) //calculate the Vx of the asteroid based on the size of the asteroid, bigger asteroids move slowly and vise versa
{
	float speed;
	
	if(size <= 75 && size > 60)
	{
		speed = 0.3;
	}
	else
	if(size <= 59 && size > 50)
	{
		speed = 0.5;
	}
	else
	if(size <= 49 && size > 40)
	{
		speed = 0.7;
	}
	else
	if(size <= 39 && size > 29)
	{
		speed = 0.9;
	}
	
	if(speed < 0.3) //increases the lower value of randSpeed to stop really slow asteroids
	{
		speed = speed + 0.3;
	}
	
	float rads = toRads(heading);
	return speed * cos(rads + 1.5708);
	
}

float calcAstSizeVy(float heading, int size) //calculate the Vy of the asteroid based on the size of the asteroid
{
	float speed;
	
	if(size <= 75 && size > 60)
	{
		speed = 0.3;
	}
	else
	if(size <= 59 && size > 50)
	{
		speed = 0.5;
	}
	else
	if(size <= 49 && size > 40)
	{
		speed = 0.7;
	}
	else
	if(size <= 39 && size > 29)
	{
		speed = 0.9;
	}
	
	if(speed < 0.3) //increases the lower value of randSpeed to stop really slow asteroids
	{
		speed = speed + 0.3;
	}
	
	float rads = toRads(heading);
	return speed * -(sin(rads + 1.5708));
}







float calcAstRandHeading(void) //calculates a random value for an asteroid's heading between 0 and 360
{
	srand(time(0));
	
	float randHeading = rand()%(360-0 + 1) + 0;
	return randHeading;
}

float calcAstDirectionalHeading(float x) //adjust's an asteroid's heading based on whether the asteroid spawns off the left/right side of the screen, adjusts so all asteroids head inwards to the screen
{
	float randHeading;
	
	srand(time(0));
	
	if(x < 0) //spawn off left side of screen
	{
		
		randHeading = rand()%(340-180 + 1) + 180; //180-340 degrees
	}
	else
	if(x > 480) //spawn off right side of screen
	{
		randHeading = rand()%(160-20 + 1) + 20; //20-160 degrees
	}
	

	return randHeading;
}

int calcAstRandSize(void) //calcualtes a random size for the asteroid
{
	srand(time(0));
	float randSize = rand()%(75-30 + 1) + 30;
	return randSize;
}






float toRads(float deg) 
{
	return deg * (PI / 180);
}


void checkMissileAsteroidCollision(struct particle *ast, struct particle *mis) //handles collisions between missiles and asteroids
{
		float missileX = 0;
		float missileY = 0;
	
		float asteroidX = 0;
		float asteroidY = 0;
		float asteroidSize = 0;
	
	
		for( ; mis ; mis = mis->next ) //iterates through the list of missiles passed through to the function
		{
				missileX = mis->x;
				missileY = mis->y;
			
			for(struct particle *ast2 = ast ; ast2 ; ast2 = ast2->next) //for each missile, tests whether or not each asteroid in existence is in collision with the missile in question
			{
				asteroidX = ast2->x;
				asteroidY = ast2->y;
				asteroidSize = ast2->size;
			
				
				if(missileX < asteroidX + asteroidSize &&
					 missileX > asteroidX &&
					 missileY < asteroidY + asteroidSize &&
					 missileY > asteroidY && mis->draw == 1 && ast2->draw == 1) //tests boundaries of asteroid and missiles in question
				{
					if(ast2->cluster == 1) //if the asteroid is a first generation asteroid i.e. has not been hit by a missile previously, causes the asteroid to split into two smaller asteroids
					{
						clusterAsteroid(asteroidX, asteroidY, asteroidSize, ast2->vx, ast2->vy); //creates cluster of two smaller asteroids based on parent asteroid's location
						clusterCounter++; //debug
						
					}
					
					collisionCounter++; //debug
					ast2->draw = 0; //remove asteroid from play
					mis->draw = 0; //remove missile from play
					incScore(); //increase score on successful collision
					
					
					
				}
			}
			
		}
}

void checkShipAsteroidCollision(struct particle *ast) //handles collisions between ship and asteroids
{
		float asteroidX = 0;
		float asteroidY = 0;
		float asteroidSize = 0;
	
		for(struct particle *ast2 = ast; ast2; ast2 = ast2->next) //iterates through list of active asteroids
		{
			asteroidX = ast2->x;
			asteroidY = ast2->y;
			asteroidSize = ast2->size;
			
			if(ship.x < asteroidX + asteroidSize &&
				 ship.x > asteroidX &&
				 ship.y < asteroidY + asteroidSize &&
				 ship.y > asteroidY && ast2->draw == 1) //tests ship/asteroid boundaries 
			{
				ast2->draw = 0; //remove asteroid from play
				collisionCounter++;
				decLives(); //decrease lives on successful collision

				ship.x = 280; //reset position of ship on collision 
				ship.y = 135;
				
				setMenuTrue(); //enable the menu
			}
		}
}

void wrap(struct particle *mis, struct particle *ast) //if the ship leaves the screen, wrap the position of the ship around to the opposite edge of the screen
{
	if(ship.x > 480)
	{
		ship.x = 0;
	}
	
	if(ship.x < 0)
	{
		ship.x = 480;
	}
	
	if(ship.y > 270)
	{
		ship.y = 0;
	}
	
	if(ship.y < 0)
	{
		ship.y = 270;
	}
}

void clusterAsteroid(float astX, float astY, int astSize, float astVx, float astVy) //creates two smaller asteroid in a "cluster" when 1st generation asteroids are destroyed by missiles
{
	
				struct particle *astCluster = allocnodeA(); //create new asteroid 
        if(astCluster) 
					{
            
            astCluster->next = activeAsteroids;
            activeAsteroids = astCluster;
            initAsteroidCluster(astCluster, astX, astY, astSize, calcAstRandHeading(), astVx, astVy); //initalise asteroid parameters
					}  
			

				
				
				struct particle *astCluster2 = allocnodeA(); //create new asteroid 
				if(astCluster2) 
					{
           
            astCluster2->next = activeAsteroids;
            activeAsteroids = astCluster2;
            initAsteroidCluster(astCluster2, (astX+10), (astY+10), astSize, calcAstRandHeading(), -(astVx), -(astVy)); //initalise asteroid parameters, velocity is reversed
					}					
				
					
					
}









//Getter functions 
float getHeading(void)
{
	return ship.heading;
}

float getHeadingRads(void)
{
	return ship.heading * (PI / 180);
}

float getShipX(void) //ship origin
{
	return ship.x;
}

float getShipY(void) //ship origin
{
	return ship.y;
}

float getShipX1(void) //ship borders
{
	return ship.x1;
}

float getShipY1(void) //ship borders
{
	return ship.y1;
}

float getShipX2(void) //ship borders
{
	return ship.x2;
}

float getShipY2(void) //ship borders
{
	return ship.y2;
}

float getShipX3(void) //ship borders
{
	return ship.x3;
}

float getShipY3(void) //ship borders
{
	return ship.y3;
}

float getShipVx(void)
{
	return ship.vx;
}

float getShipVy(void)
{
	return ship.vy;
}

struct particle* getMissileList(void)
{
	return activeMissiles;
}

struct particle* getAsteroidList(void)
{
	return activeAsteroids;
}

int getCollisionCounter(void)
{
	return collisionCounter;
}

int getClusterCounter(void)
{
	return clusterCounter;
}

int getAsteroidSpawnRate(void)
{
	return asteroidSpawnRate;
}

int getIncDifficulty(void)
{
	return incDifficulty;
}



//Functions associated with the asteroid particle system

void newAsteroid(void) //creates a new asteroid, adds it to the list of active asteroids and initalises its parameters
{
        struct particle *asteroid = allocnodeA();
        if(asteroid) 
					{
            /* put the new particle at the front of the list */
            asteroid->next = activeAsteroids;
            activeAsteroids = asteroid;
            initAsteroid(asteroid);
					}     
}

void initAsteroid(struct particle *a) //initalises an asteroids parameters
{
    
		a->x = calcAstRandX(); //calculates random x spawn
    a->y = calcAstRandY(); //calculates random y spawn
		a->heading = calcAstDirectionalHeading(a->x); //calculates heading based on spawn location
		a->size = calcAstRandSize(); //calcuates random size
    a->vx = calcAstRandVx(a->heading); //calculates random x velocity based on heading
    a->vy = calcAstRandVy(a->heading); //calculates random y velocity based on heading
		
		a->cluster = 1; //1st gen asteroid enables clustering 
		a->draw = 1; //if 1, draw the asteroid
    a->dur = 30; //duration of asteroid life
	
		
}

void initAsteroidCluster(struct particle *a, float astX, float astY, int astSize, float heading, float astVx, float astVy) //initalises a child asteroid with slightly different parameters to the parent asteroid
{
    
		a->x = astX; //X coord of child asteroid same as parent
    a->y = astY; //Y coord of child asteroid same as parent
		a->heading = calcAstRandHeading(); //calculates random heading
		a->size = (astSize / 2); //child asteroids are half the size of parent asteroids 
    
		a->vx = astVx; //Vx and Vy are identical to parent asteroid
		a->vy = astVy;
		
		a->draw = 1; //if 1, draw the asteroid
		a->cluster = 0; //disable further clustering
    a->dur = 20; //duration of asteroid life
	
		
}


void updateAsteroids(struct particle *l) //updates asteroid positions and duration
{
    for( ; l ; l = l->next ) //iterates through the list of asteroids passed through to the function
		{
			
				l->x = l->x + l->vx; //adjusts asteroid x and y coordinates based on previous coords and velocity 
				l->y = l->y + l->vy;
			
				l->dur -= Dt; //reduces duration of asteroid 
			
			
				if( l->next->dur<=0 ) //if asteroid duration reaches zero, the asteroid node is removed from the linked list
				{
            struct particle *expired = l->next;
            l->next = l->next->next;
            freenodeA(expired);
        }
		}
}

//particle system functions - missiles

void newMissile(void) //function creates new missiles and adds them to the list of existing missiles
{
        struct particle *missile = allocnodeM();
        if(missile) 
					{
            /* put the new particle at the front of the list */
            missile->next = activeMissiles;
            activeMissiles = missile;
            initMissile(missile); //initalise missile variables 
					}     
}

void initMissile(struct particle *m) //initalises a missiles variables 
{
    m->x = getShipX1(); //missiles X and Y coordinates are set to the ship's coordinates
		m->y = getShipY1();
		m->heading = ship.heading; //missile heading is identical to ship's heading
    m->vx = calcMissileVx(); //calculate missile Vx and Vy based on ship's heading 
    m->vy = calcMissileVy();
		m->draw = 1; //draw the missile
		
    m->dur = 5; //Duration of missile's life
}


void updateMissiles(struct particle *l) //update the existing missile's coordinates
{
    for( ; l ; l = l->next ) //iterates through the list of active missiles
		{
			
				l->x = l->x + l->vx; //updates missile x and y based on missile velocity
				l->y = l->y + l->vy;
			
				l->dur -= Dt; //decreases life of missile
			
			
				if( l->next->dur<=0 ) //if missile life reaches zero, the missile is expired and the node can be overwritten
				{
            struct particle *expired = l->next;
            l->next = l->next->next;
            freenodeM(expired);
        }
				
				if(l->next->x > 480 || l->next->x < 0) //if missile goes off either the left or right edge of the screen, the node is expired
				{
					struct particle *expired = l->next;
          l->next = l->next->next;
          freenodeM(expired);
				}
				
				if(l->next->y > 270 || l->next->y < 0) //if missile goes off either the top or bottom edge of the screen, the node is expired
				{
					struct particle *expired = l->next;
          l->next = l->next->next;
          freenodeM(expired);
				}
			
			
				
		}
}













//node functions - missiles 
void initialise_heapM(void) //initalises the missile heap according to the heapsizeM variable
{
    int n;
    for( n=0 ; n<(heapsizeM-1) ; n++) {
        heapM[n].next = &heapM[n+1];
    }
    heapM[n].next = 0;
    freenodesM = &heapM[0];
}

node_t *allocnodeM(void) //allocates memory to a node for the creation of a new missile
{
    node_t *node = 0;
    if(freenodesM) {
        node = freenodesM;
        freenodesM = freenodesM->next;
    }
    return node;
}

void freenodeM(node_t *n) //releases the memory held by a node so that it can be overwritten by a new missile
{
    n->next = freenodesM;
    freenodesM = n;
}


//node functions - asteroids

void initialise_heapA(void) //initalises the asteroid heap according to the heapsizeA variable
{
    int n;
    for( n=0 ; n<(heapsizeA-1) ; n++) {
        heapA[n].next = &heapA[n+1];
    }
    heapA[n].next = 0;
    freenodesA = &heapA[0];
}

node_t *allocnodeA(void) //allocates memory to a node for the creation of a new asteroid
{
    node_t *node = 0;
    if(freenodesA) {
        node = freenodesA;
        freenodesA = freenodesA->next;
    }
    return node;
}

void freenodeA(node_t *n) //releases the memory held by a node so that it can be overwritten by a new asteroid
{
    n->next = freenodesA;
    freenodesA = n;
}


void asteroidTick(void) //increases the asteroidTickCount variable and creates new asteroids when it reaches 100
{
	asteroidTickCount++;


	
	if(asteroidTickCount > asteroidSpawnRate) //at 100, an asteroid is spawned and asteroidTickCount is reset to 0
	{
		newAsteroid();
		asteroidTickCount = 0;
		incScore();
		//incDifficulty++;
	}

	
}

