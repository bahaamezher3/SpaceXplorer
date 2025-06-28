# SpaceXplorer V2

A terminal-based space exploration game written in C where you pilot a spaceship through a procedurally generated world, collect valuable space junk, avoid dangerous asteroids, and manage your ship's resources.

## 🚀 Features

- **Dynamic World Generation**: Procedurally generated space environments with configurable dimensions
- **Resource Management**: Collect and manage different types of space junk (Metal, Plastic, Electronics, Fuel Cells)
- **Multiple Difficulty Levels**: Easy, Medium, and Hard modes with varying challenges
- **Asteroid Avoidance**: Navigate around moving asteroids that can damage your ship
- **Ship Status System**: Monitor fuel, health, and collected resources
- **Leaderboard System**: Track high scores across different difficulty levels
- **Impassable Obstacles**: Navigate around static barriers in the space environment
- **Real-time Game Loop**: Smooth gameplay with continuous world updates

## 🎮 Gameplay

### Objective
Collect space junk to earn points while avoiding asteroids and managing your ship's fuel and health. Reach the target score for your chosen difficulty level to win!

### Controls
- **W/A/S/D** or **Arrow Keys**: Move your spaceship
- **Q**: Quit the game
- **U**: Use collected junk items
- **L**: View leaderboard

### Game Elements
- **@**: Your spaceship
- **A**: Moving asteroid (dangerous!)
- **M**: Metal junk (10 points)
- **P**: Plastic junk (5 points)
- **E**: Electronics junk (15 points)
- **F**: Fuel Cell junk (20 points)
- **#**: Impassable obstacle

### Difficulty Levels
- **Easy**: 500 fuel, 40 junk items, 500 points to win
- **Medium**: 350 fuel, 30 junk items, 750 points to win
- **Hard**: 200 fuel, 20 junk items, 1000 points to win

## 🛠️ Installation & Build

### Prerequisites
- CMake 3.24 or higher
- C compiler with C11 support (GCC, Clang, or MSVC)

### Building the Game

1. **Clone the repository**
   ```bash
   git clone <repository-url>
   cd spaceXplorer
   ```

2. **Create build directory**
   ```bash
   mkdir build
   cd build
   ```

3. **Configure and build**
   ```bash
   cmake ..
   cmake --build .
   ```

4. **Run the game**
   ```bash
   ./spaceXplorerV2
   ```

### Alternative: Direct Compilation
If you prefer to compile directly without CMake:
```bash
gcc -std=c11 -o spaceXplorerV2 main.c game.c
./spaceXplorerV2
```

## 📁 Project Structure

```
spaceXplorer/
├── main.c              # Main game entry point
├── game.c              # Core game logic implementation
├── game.h              # Game structures and function declarations
├── CMakeLists.txt      # CMake build configuration
├── config.txt          # World configuration (auto-generated)
├── leaderboard.txt     # High scores (auto-generated)
└── intro.txt          # Game introduction text (optional)
```

## 🎯 How to Play

1. **Start the Game**: Run the executable and enter your name
2. **Choose Difficulty**: Select Easy, Medium, or Hard
3. **Navigate**: Use WASD or arrow keys to move your spaceship
4. **Collect Junk**: Move over junk items to collect them and earn points
5. **Avoid Asteroids**: Stay away from moving asteroids that can damage your ship
6. **Manage Resources**: Monitor your fuel and health levels
7. **Use Items**: Press 'U' to use collected junk for various benefits
8. **Win Condition**: Reach the target score for your difficulty level

## 🔧 Configuration

The game automatically creates a `config.txt` file on first run with default settings:
```
width=18
height=18
```

You can modify these values to change the world size (minimum 18x18).

## 🏆 Leaderboard

The game maintains a persistent leaderboard across sessions, tracking:
- Player name
- Score achieved
- Difficulty level

View the leaderboard anytime during gameplay by pressing 'L'.

## 🐛 Troubleshooting

### Common Issues

**Build Errors:**
- Ensure you have CMake 3.24+ installed
- Verify your C compiler supports C11 standard
- Check that all source files are present

**Runtime Issues:**
- The game requires a terminal that supports ANSI escape codes
- Ensure you have write permissions in the game directory for save files

### Platform Support
- **Windows**: Tested with MinGW and MSVC
- **Linux**: Tested with GCC
- **macOS**: Tested with Clang

## 🤝 Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

### Development Guidelines
- Follow the existing code style
- Add comments for complex logic
- Test on multiple platforms
- Update documentation for new features

## 📄 License

This project is open source. Feel free to use, modify, and distribute according to your needs.

## 🎮 Game Mechanics Details

### Fuel System
- Fuel consumption varies by difficulty
- Running out of fuel ends the game
- Fuel Cells can be used to restore fuel

### Health System
- Colliding with asteroids reduces health
- Health cannot be restored during gameplay
- Zero health ends the game

### Scoring System
- Different junk types have different point values
- Higher difficulty levels require more points to win
- Scores are saved to the leaderboard

### Asteroid Behavior
- Asteroids move continuously across the screen
- Movement patterns are randomized
- Collision detection prevents overlapping with other objects

---

**Enjoy exploring space!** 🚀✨ 