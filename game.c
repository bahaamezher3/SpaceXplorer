#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "game.h"

const int FUEL_LEVELS[] = {500, 350, 200};
const int FUEL_CONSUMPTION[] = {1, 2, 3};
const int JUNK_COUNTS[] = {40, 30, 20};
const int ASTEROID_SPEEDS[] = {1, 2, 3};
const int WIN_SCORES[] = {500, 750, 1000};

const char* CONFIG_FILE = "config.txt";
const char* LEADERBOARD_FILE = "leaderboard.txt";
const char* INTRO_FILE = "intro.txt";

void initGame(Game* game) {
    char difficultyChar;
    
    printf("Enter your name (max %d characters): ", MAX_NAME_LENGTH - 1);
    fgets(game->playerName, MAX_NAME_LENGTH, stdin);
    game->playerName[strcspn(game->playerName, "\n")] = 0;
    
    do {
        printf("Choose difficulty (E)asy, (M)edium, (H)ard: ");
        scanf(" %c", &difficultyChar);
        difficultyChar = toupper(difficultyChar);
    } while (difficultyChar != 'E' && difficultyChar != 'M' && difficultyChar != 'H');
    
    switch (difficultyChar) {
        case 'E': game->difficulty = EASY; break;
        case 'M': game->difficulty = MEDIUM; break;
        case 'H': game->difficulty = HARD; break;
        default: game->difficulty = MEDIUM; break;
    }
    
    while (getchar() != '\n');
    
    loadConfig(game);
    
    createWorld(game);
    
    game->ship.position.x = game->worldWidth / 2;
    game->ship.position.y = game->worldHeight / 2;
    game->ship.fuel = FUEL_LEVELS[game->difficulty];
    game->ship.maxFuel = FUEL_LEVELS[game->difficulty];
    game->ship.health = 100;
    game->ship.maxHealth = 100;
    game->ship.metal = 0;
    game->ship.plastic = 0;
    game->ship.electronics = 0;
    game->ship.fuelCells = 0;
    
    int edge = rand() % 4;
    
    switch (edge) {
        case 0:
            game->asteroid.position.x = rand() % game->worldWidth;
            game->asteroid.position.y = 0;
            game->asteroid.direction.x = (rand() % 3) - 1;
            game->asteroid.direction.y = 1;
            break;
        case 1:
            game->asteroid.position.x = game->worldWidth - 1;
            game->asteroid.position.y = rand() % game->worldHeight;
            game->asteroid.direction.x = -1;
            game->asteroid.direction.y = (rand() % 3) - 1;
            break;
        case 2:
            game->asteroid.position.x = rand() % game->worldWidth;
            game->asteroid.position.y = game->worldHeight - 1;
            game->asteroid.direction.x = (rand() % 3) - 1;
            game->asteroid.direction.y = -1;
            break;
        case 3:
            game->asteroid.position.x = 0;
            game->asteroid.position.y = rand() % game->worldHeight;
            game->asteroid.direction.x = 1;
            game->asteroid.direction.y = (rand() % 3) - 1;
            break;
    }
    
    if (game->asteroid.direction.x == 0 && game->asteroid.direction.y == 0) {
        game->asteroid.direction.x = 1;
    }
    
    game->asteroid.symbol = 'A';
    
    for (int i = 0; i < IMPASSABLE_CELLS; i++) {
        int valid = 0;
        while (!valid) {
            int x = rand() % game->worldWidth;
            int y = rand() % game->worldHeight;
            
            if ((x != game->ship.position.x || y != game->ship.position.y) &&
                (x != game->asteroid.position.x || y != game->asteroid.position.y)) {
                
                int overlap = 0;
                for (int j = 0; j < i; j++) {
                    if (game->impassableCells[j].position.x == x && 
                        game->impassableCells[j].position.y == y) {
                        overlap = 1;
                        break;
                    }
                }
                
                if (!overlap) {
                    game->impassableCells[i].position.x = x;
                    game->impassableCells[i].position.y = y;
                    game->impassableCells[i].symbol = '#';
                    valid = 1;
                }
            }
        }
    }
    
    game->junkCount = JUNK_COUNTS[game->difficulty];
    
    for (int i = 0; i < game->junkCount; i++) {
        int valid = 0;
        while (!valid) {
            int x = rand() % game->worldWidth;
            int y = rand() % game->worldHeight;
            
            if ((x != game->ship.position.x || y != game->ship.position.y) &&
                (x != game->asteroid.position.x || y != game->asteroid.position.y)) {
                
                int overlap = 0;
                
                for (int j = 0; j < IMPASSABLE_CELLS; j++) {
                    if (game->impassableCells[j].position.x == x && 
                        game->impassableCells[j].position.y == y) {
                        overlap = 1;
                        break;
                    }
                }
                
                if (!overlap) {
                    for (int j = 0; j < i; j++) {
                        if (game->junkItems[j].position.x == x && 
                            game->junkItems[j].position.y == y) {
                            overlap = 1;
                            break;
                        }
                    }
                }
                
                if (!overlap) {
                    game->junkItems[i].position.x = x;
                    game->junkItems[i].position.y = y;
                    game->junkItems[i].type = rand() % 4;
                    game->junkItems[i].collected = 0;
                    
                    switch (game->junkItems[i].type) {
                        case METAL:
                            game->junkItems[i].value = 10;
                            game->junkItems[i].symbol = 'M';
                            break;
                        case PLASTIC:
                            game->junkItems[i].value = 5;
                            game->junkItems[i].symbol = 'P';
                            break;
                        case ELECTRONICS:
                            game->junkItems[i].value = 15;
                            game->junkItems[i].symbol = 'E';
                            break;
                        case FUEL_CELL:
                            game->junkItems[i].value = 20;
                            game->junkItems[i].symbol = 'F';
                            break;
                    }
                    
                    valid = 1;
                }
            }
        }
    }
    
    game->score = 0;
    game->isGameOver = 0;
    game->hasWon = 0;
}

void loadConfig(Game* game) {
    FILE* file = fopen(CONFIG_FILE, "r");
    
    game->worldWidth = WORLD_MIN_SIZE;
    game->worldHeight = WORLD_MIN_SIZE;
    
    if (file != NULL) {
        fscanf(file, "width=%d\n", &game->worldWidth);
        fscanf(file, "height=%d\n", &game->worldHeight);
        
        fclose(file);
        
        if (game->worldWidth < WORLD_MIN_SIZE) game->worldWidth = WORLD_MIN_SIZE;
        if (game->worldHeight < WORLD_MIN_SIZE) game->worldHeight = WORLD_MIN_SIZE;
    } else {
        file = fopen(CONFIG_FILE, "w");
        if (file != NULL) {
            fprintf(file, "width=%d\n", game->worldWidth);
            fprintf(file, "height=%d\n", game->worldHeight);
            fclose(file);
        }
    }
}

void createWorld(Game* game) {
    game->world = (char**)malloc(game->worldHeight * sizeof(char*));
    for (int y = 0; y < game->worldHeight; y++) {
        game->world[y] = (char*)malloc(game->worldWidth * sizeof(char));
    }
}

void cleanupGame(Game* game) {
    // Free world memory
    for (int y = 0; y < game->worldHeight; y++) {
        free(game->world[y]);
    }
    free(game->world);
}

void renderWorld(Game* game) {
    system("cls");
    
    for (int y = 0; y < game->worldHeight; y++) {
        for (int x = 0; x < game->worldWidth; x++) {
            game->world[y][x] = '.';
        }
    }
    
    game->world[game->ship.position.y][game->ship.position.x] = 'S';
    
    game->world[game->asteroid.position.y][game->asteroid.position.x] = game->asteroid.symbol;
    
    for (int i = 0; i < IMPASSABLE_CELLS; i++) {
        int x = game->impassableCells[i].position.x;
        int y = game->impassableCells[i].position.y;
        game->world[y][x] = game->impassableCells[i].symbol;
    }
    
    for (int i = 0; i < game->junkCount; i++) {
        if (!game->junkItems[i].collected) {
            int x = game->junkItems[i].position.x;
            int y = game->junkItems[i].position.y;
            game->world[y][x] = game->junkItems[i].symbol;
        }
    }
    
    printf("\n   ");
    for (int x = 0; x < game->worldWidth; x++) {
        printf("%d", x % 10);
    }
    printf("\n");
    
    for (int y = 0; y < game->worldHeight; y++) {
        printf("%2d ", y % 100);
        for (int x = 0; x < game->worldWidth; x++) {
            printf("%c", game->world[y][x]);
        }
        printf("\n");
    }
    
    printf("\nFuel: %d/%d | Health: %d/%d | Score: %d\n", 
           game->ship.fuel, game->ship.maxFuel, 
           game->ship.health, game->ship.maxHealth, 
           game->score);
           
    printf("\nControls: (W)Up (S)Down (A)Left (D)Right (Q)Quit (I)Inventory (U)Use items\n");
}

void handleInput(Game* game) {
    char input;
    printf("\nEnter command: ");
    scanf(" %c", &input);
    input = toupper(input);
    
    switch (input) {
        case 'W':
            moveSpaceship(game, 0, -1);
            break;
        case 'S':
            moveSpaceship(game, 0, 1);
            break;
        case 'A':
            moveSpaceship(game, -1, 0);
            break;
        case 'D':
            moveSpaceship(game, 1, 0);
            break;
        case 'I':
            displayShipStatus(game);
            break;
        case 'U':
            printf("Choose item to use:\n");
            printf("1. Metal (Repair ship)\n");
            printf("2. Fuel Cell (Refuel ship)\n");
            printf("3. Cancel\n");
            
            int choice;
            scanf("%d", &choice);
            if (choice >= 1 && choice <= 2) {
                useJunk(game, choice);
            }
            break;
        case 'Q':
            game->isGameOver = 1;
            break;
    }
}

void moveSpaceship(Game* game, int dx, int dy) {
    int newX = game->ship.position.x + dx;
    int newY = game->ship.position.y + dy;
    
    if (newX >= 0 && newX < game->worldWidth && newY >= 0 && newY < game->worldHeight) {
        int canMove = 1;
        
        for (int i = 0; i < IMPASSABLE_CELLS; i++) {
            if (newX == game->impassableCells[i].position.x && 
                newY == game->impassableCells[i].position.y) {
                canMove = 0;
                break;
            }
        }
        
        if (canMove) {
            game->ship.position.x = newX;
            game->ship.position.y = newY;
            
            game->ship.fuel -= FUEL_CONSUMPTION[game->difficulty];
            
            if (game->ship.fuel <= 0) {
                game->isGameOver = 1;
                game->hasWon = 0;
                return;
            }
            
            moveAsteroid(game);
            
            checkCollisions(game);
        }
    }
}

void moveAsteroid(Game* game) {
    int speed = ASTEROID_SPEEDS[game->difficulty];
    
    for (int i = 0; i < speed; i++) {
        int newX = game->asteroid.position.x + game->asteroid.direction.x;
        int newY = game->asteroid.position.y + game->asteroid.direction.y;
        
        if (newX < 0 || newX >= game->worldWidth) {
            game->asteroid.direction.x *= -1;
            newX = game->asteroid.position.x + game->asteroid.direction.x;
        }
        
        if (newY < 0 || newY >= game->worldHeight) {
            game->asteroid.direction.y *= -1;
            newY = game->asteroid.position.y + game->asteroid.direction.y;
        }
        
        int blocked = 0;
        for (int j = 0; j < IMPASSABLE_CELLS; j++) {
            if (newX == game->impassableCells[j].position.x && 
                newY == game->impassableCells[j].position.y) {
                blocked = 1;
                break;
            }
        }
        
        if (blocked) {
            game->asteroid.direction.x *= -1;
            game->asteroid.direction.y *= -1;
            
            if (game->asteroid.direction.x == 0 && game->asteroid.direction.y == 0) {
                game->asteroid.direction.x = 1;
            }
            
            newX = game->asteroid.position.x + game->asteroid.direction.x;
            newY = game->asteroid.position.y + game->asteroid.direction.y;
        }
        
        game->asteroid.position.x = newX;
        game->asteroid.position.y = newY;
        
        if (game->asteroid.position.x == game->ship.position.x && 
            game->asteroid.position.y == game->ship.position.y) {
            game->isGameOver = 1;
            game->hasWon = 0;
            break;
        }
    }
}

void checkCollisions(Game* game) {
    for (int i = 0; i < game->junkCount; i++) {
        if (!game->junkItems[i].collected &&
            game->ship.position.x == game->junkItems[i].position.x && 
            game->ship.position.y == game->junkItems[i].position.y) {
            collectJunk(game, i);
        }
    }
    
    if (game->score >= WIN_SCORES[game->difficulty]) {
        game->isGameOver = 1;
        game->hasWon = 1;
    }
}

void collectJunk(Game* game, int index) {
    game->junkItems[index].collected = 1;
    
    game->score += game->junkItems[index].value;
    
    switch (game->junkItems[index].type) {
        case METAL:
            game->ship.metal++;
            printf("Collected metal!\n");
            break;
        case PLASTIC:
            game->ship.plastic++;
            printf("Collected plastic!\n");
            break;
        case ELECTRONICS:
            game->ship.electronics++;
            printf("Collected electronics!\n");
            break;
        case FUEL_CELL:
            game->ship.fuelCells++;
            printf("Collected fuel cell!\n");
            break;
    }
    
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

void useJunk(Game* game, int option) {
    switch (option) {
        case 1:
            if (game->ship.metal > 0) {
                game->ship.health += 10;
                if (game->ship.health > game->ship.maxHealth) {
                    game->ship.health = game->ship.maxHealth;
                }
                game->ship.metal--;
                printf("Ship repaired! Health: %d/%d\n", game->ship.health, game->ship.maxHealth);
            } else {
                printf("Not enough metal!\n");
            }
            break;
        case 2:
            if (game->ship.fuelCells > 0) {
                game->ship.fuel += 50;
                if (game->ship.fuel > game->ship.maxFuel) {
                    game->ship.fuel = game->ship.maxFuel;
                }
                game->ship.fuelCells--;
                printf("Ship refueled! Fuel: %d/%d\n", game->ship.fuel, game->ship.maxFuel);
            } else {
                printf("Not enough fuel cells!\n");
            }
            break;
    }
    printf("Press Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

void displayShipStatus(Game* game) {
    printf("\n=== SHIP STATUS ===\n");
    printf("Fuel: %d/%d\n", game->ship.fuel, game->ship.maxFuel);
    printf("Health: %d/%d\n", game->ship.health, game->ship.maxHealth);
    printf("Score: %d\n", game->score);
    printf("\n=== INVENTORY ===\n");
    printf("Metal: %d\n", game->ship.metal);
    printf("Plastic: %d\n", game->ship.plastic);
    printf("Electronics: %d\n", game->ship.electronics);
    printf("Fuel Cells: %d\n", game->ship.fuelCells);
    
    printf("\nScore needed to win: %d\n", WIN_SCORES[game->difficulty]);
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');
    getchar();
}

void displayWelcomeMessage() {
    printf("\n========================================\n");
    printf("        WELCOME TO SPACEXPLORER        \n");
    printf("========================================\n\n");
    
    FILE* file = fopen(INTRO_FILE, "r");
    if (file != NULL) {
        char line[100];
        while (fgets(line, sizeof(line), file)) {
            printf("%s", line);
        }
        fclose(file);
    } else {
        printf("You are an intrepid space explorer lost in deep space.\n");
        printf("Your mission is to collect space junk, avoid the dangerous asteroid,\n");
        printf("and find your way back home.\n\n");
        printf("Collect enough resources to win, but be careful of your fuel supply!\n");

        file = fopen(INTRO_FILE, "w");
        if (file != NULL) {
            fprintf(file, "You are an intrepid space explorer lost in deep space.\n");
            fprintf(file, "Your mission is to collect space junk, avoid the dangerous asteroid,\n");
            fprintf(file, "and find your way back home.\n\n");
            fprintf(file, "Collect enough resources to win, but be careful of your fuel supply!\n");
            fclose(file);
        }
    }
    printf("\nPress Enter to start your adventure...");
    getchar();
}

void displayEndGameMessage(Game* game) {
    printf("\n========================================\n");
    
    if (game->hasWon) {
        printf("             YOU WIN!                 \n");
        printf("========================================\n\n");
        printf("Congratulations, %s!\n", game->playerName);
        printf("You have collected enough resources and found your way home!\n");
        printf("Final Score: %d\n", game->score);
    } else {
        printf("             GAME OVER                \n");
        printf("========================================\n\n");
        
        if (game->ship.fuel <= 0) {
            printf("Your spaceship ran out of fuel and is now drifting forever in space.\n");
        } else {
            printf("Your spaceship was hit by the asteroid and was destroyed.\n");
        }
        
        printf("Final Score: %d\n", game->score);
    }
    
    saveScore(game);
    
    displayLeaderboard();
    
    printf("\nPress Enter to exit...");
    getchar();
}

void saveScore(Game* game) {
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int count = 0;
    
    loadLeaderboard(leaderboard, &count);
    
    if (count < MAX_LEADERBOARD_ENTRIES || game->score > leaderboard[count - 1].score) {
        LeaderboardEntry newEntry;
        strcpy(newEntry.playerName, game->playerName);
        newEntry.score = game->score;
        newEntry.difficulty = game->difficulty;
        
        int insertIndex = count;
        
        if (count < MAX_LEADERBOARD_ENTRIES) {
            count++;
        }
        
        for (int i = 0; i < count; i++) {
            if (newEntry.score > leaderboard[i].score) {
                insertIndex = i;
                break;
            }
        }
        
        for (int i = count - 1; i > insertIndex; i--) {
            leaderboard[i] = leaderboard[i - 1];
        }
        
        leaderboard[insertIndex] = newEntry;
        
        if (count > MAX_LEADERBOARD_ENTRIES) {
            count = MAX_LEADERBOARD_ENTRIES;
        }
        saveLeaderboard(leaderboard, count);
    }
}

void loadLeaderboard(LeaderboardEntry leaderboard[], int* count) {
    FILE* file = fopen(LEADERBOARD_FILE, "r");
    *count = 0;
    
    if (file != NULL) {
        while (*count < MAX_LEADERBOARD_ENTRIES && 
               fscanf(file, "%19[^,],%d,%d\n", 
                     leaderboard[*count].playerName,
                     &leaderboard[*count].score,
                     (int*)&leaderboard[*count].difficulty) == 3) {
            (*count)++;
        }
        
        fclose(file);
    }
}

void saveLeaderboard(LeaderboardEntry leaderboard[], int count) {
    FILE* file = fopen(LEADERBOARD_FILE, "w");
    
    if (file != NULL) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "%s,%d,%d\n", 
                   leaderboard[i].playerName,
                   leaderboard[i].score,
                   leaderboard[i].difficulty);
        }
        
        fclose(file);
    }
}

void displayLeaderboard() {
    LeaderboardEntry leaderboard[MAX_LEADERBOARD_ENTRIES];
    int count = 0;
    loadLeaderboard(leaderboard, &count);
    
    if (count > 0) {
        printf("\n============ LEADERBOARD ============\n");
        printf("Rank | Name          | Score | Difficulty\n");
        printf("-------------------------------------\n");
        
        for (int i = 0; i < count; i++) {
            char diffChar;
            switch (leaderboard[i].difficulty) {
                case EASY: diffChar = 'E'; break;
                case MEDIUM: diffChar = 'M'; break;
                case HARD: diffChar = 'H'; break;
                default: diffChar = '?'; break;
            }
            
            printf("%-4d | %-14s | %-5d | %c\n", 
                   i + 1, 
                   leaderboard[i].playerName, 
                   leaderboard[i].score,
                   diffChar);
        }
    } else {
        printf("\nNo high scores yet.\n");
    }
}

void updateGame(Game* game) {
} 