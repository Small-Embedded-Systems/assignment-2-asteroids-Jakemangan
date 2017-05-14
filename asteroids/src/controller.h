
//Variables for handling joystick input on relevant pins
static DigitalIn left(P5_0);
static DigitalIn right(P5_4);
static DigitalIn up(P5_2);
static DigitalIn down(P5_1);
static DigitalIn center(P5_3);

void userinput(void);
void testButtons(void);
void testUserInput(void);
void testMenuInput(void);