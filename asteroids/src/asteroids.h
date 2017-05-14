
//GETTERS
int getScore(void);
int getLives(void);
bool getMenu(void);
bool getEndGame(void);
bool getMenuState(void);
bool getLifeLost(void);

//MUTATORS
void incScore(void);
void decLives(void);
void disableMenu(void);
void resetGame(void);
void setMenuTrue(void);
void setMenuFalse(void);
void setLifeLostTrue(void);
void setLifeLostFalse(void);
void setEndGameTrue(void);
void setEndGameFalse(void);
bool returnEndGame(void);
void restartGame(void);