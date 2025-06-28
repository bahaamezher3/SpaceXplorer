#ifndef SPACEXPLORER_GAME_H
#define SPACEXPLORER_GAME_H

#define WORLD_MIN_SIZE 18
#define MAX_NAME_LENGTH 20
#define MAX_JUNK_ITEMS 40
#define MAX_LEADERBOARD_ENTRIES 10
#define IMPASSABLE_CELLS 3

typedef enum {
    EASY,
    MEDIUM,
    HARD
} Difficulty;

typedef struct {
    int x;
    int y;
} Position;

typedef enum {
    METAL,
    PLASTIC,
    ELECTRONICS,
    FUEL_CELL
} JunkType;

typedef struct {
    Position position;
    JunkType type;
    int value;
    char symbol;
    int collected;
} SpaceJunk;

typedef struct {
    Position position;
    int fuel;
    int maxFuel;
    int health;
    int maxHealth;
    int metal;
    int plastic;
    int electronics;
    int fuelCells;
} Spaceship;

typedef struct {
    Position position;
    Position direction;
    char symbol;
} Asteroid;

typedef struct {
    Position position;
    char symbol;
} ImpassableCell;

typedef struct {
    int worldWidth;
    int worldHeight;
    char** world;
    Spaceship ship;
    Asteroid asteroid;
    SpaceJunk junkItems[MAX_JUNK_ITEMS];
    int junkCount;
    ImpassableCell impassableCells[IMPASSABLE_CELLS];
    int score;
    int isGameOver;
    int hasWon;
    Difficulty difficulty;
    char playerName[MAX_NAME_LENGTH];
} Game;

typedef struct {
    char playerName[MAX_NAME_LENGTH];
    int score;
    Difficulty difficulty;
} LeaderboardEntry;

void initGame(Game* game);
void loadConfig(Game* game);
void saveScore(Game* game);
void loadLeaderboard(LeaderboardEntry leaderboard[], int* count);
void saveLeaderboard(LeaderboardEntry leaderboard[], int count);
void createWorld(Game* game);
void renderWorld(Game* game);
void handleInput(Game* game);
void updateGame(Game* game);
void moveSpaceship(Game* game, int dx, int dy);
void moveAsteroid(Game* game);
void checkCollisions(Game* game);
void collectJunk(Game* game, int index);
void useJunk(Game* game, int option);
void displayShipStatus(Game* game);
void displayWelcomeMessage();
void displayEndGameMessage(Game* game);
void displayLeaderboard();
void cleanupGame(Game* game);

#endif