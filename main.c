#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "game.h"

int main() {
    srand((unsigned int)time(NULL));
    
    displayWelcomeMessage();
    
    Game game;
    initGame(&game);
    
    while (!game.isGameOver) {
        renderWorld(&game);
        handleInput(&game);
        updateGame(&game);
    }
    
    displayEndGameMessage(&game);
    
    return 0;
} 