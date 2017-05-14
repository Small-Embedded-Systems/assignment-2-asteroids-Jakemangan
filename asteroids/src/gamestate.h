
#include <math.h> //import Math for triagnometric functions

#define PI 3.14159265358979323846 //define PI constant for mathetmical equations

typedef struct particle{ //struct that stores the information for each particle in the game, both missiles and asteroids use the same struct for information storage
    float x;
		float y;
    float vx;
		float vy;
		float heading;
    float dur;
		int size;
		int draw; //the particle will be drawn is set to 1
		int cluster; //asteroid specific - asteroid will cluster if set to 1
    struct particle *next;
} node_t;


//variables concerned with the motion of the ship
static float thrustMax = 2; 
static float thrustInc = 0.2; 
static float friction = 0.99;

//change in time
const float Dt = 0.025;

void physics(void);

void initGamestate(void);
void updateShipBorder(void);
void updateShipLocation(void);
void testGameBorders(void);
void incHeading(void);
void decHeading(void);
void rotateShip(void);
void addThrust(void);
void applyFriction(void);

float calcMissileVx(void);
float calcMissileVy(void);
float calcAstRandX(void);
float calcAstRandY(void);
float calcAstRandVx(float heading);
float calcAstRandVy(float heading);
float calcAstRandHeading(void);
float calcAstDirectionalHeading(void);
int calcAstRandSize(void);

void checkMissileAsteroidCollision(struct particle *ast, struct particle *mis);
void checkShipAsteroidCollision(struct particle *ast);
void wrap(struct particle *mis, struct particle *ast);
void clusterAsteroid(float astX, float astY, int astSize, float astVx, float astVy);
void initAsteroidCluster(struct particle *a, float astX, float astY, int astSize, float heading, float astVx, float astVy);
void asteroidTick(void);

float toRads(float heading);


//getters
float getHeading(void);
float getHeadingRads(void);
float getShipX(void);
float getShipY(void);

float getShipX1(void);
float getShipY1(void);
float getShipX2(void);
float getShipY2(void);
float getShipX3(void);
float getShipY3(void);

float getShipVy(void);
float getShipVx(void);

struct particle* getMissileList(void);
struct particle* getAsteroidList(void);

int getCollisionCounter(void);
int getClusterCounter(void);

int getAsteroidSpawnRate(void);
int getIncDifficulty(void);
	
//particle system functions - missiles
void newMissile(void);
void initMissile(struct particle *m);
void updateMissiles(struct particle *l);

//particle system functions - asteroids
void updateAsteroids(struct particle *l);
void initAsteroid(struct particle *a);
void newAsteroid(void);
void initAsteroidProperties(struct particle *a);

//node functions - missiles
void initialise_heapM(void);
node_t *allocnodeM(void);
void freenodeM(node_t *n);

//node functions - asteroids 
void initialise_heapA(void);
node_t *allocnodeA(void);
void freenodeA(node_t *n);

