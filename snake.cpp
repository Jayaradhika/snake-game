#include <iostream>
#include <deque>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <string>

// Platform-specific headers for non-blocking input
#ifdef _WIN32
  #include <conio.h>
  #include <windows.h>
  #define CLEAR "cls"
  #define SLEEP(ms) Sleep(ms)
  bool kbhit_ready() { return _kbhit() != 0; }
  char readKey()     { return (char)_getch(); }
#else
  #include <termios.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <iomanip>
  #define CLEAR "clear"
  #define SLEEP(ms) usleep((ms)*1000)

  // Set terminal to raw / non-blocking mode
  void setRawMode(bool on) {
      static struct termios oldt;
      static bool saved = false;
      if (on) {
          struct termios newt;
          if (!saved) { tcgetattr(STDIN_FILENO, &oldt); saved = true; }
          newt = oldt;
          newt.c_lflag &= ~(ICANON | ECHO);
          tcsetattr(STDIN_FILENO, TCSANOW, &newt);
          fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
      } else {
          tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
          fcntl(STDIN_FILENO, F_SETFL, 0);
      }
  }

  bool kbhit_ready() {
      char c;
      return read(STDIN_FILENO, &c, 1) == 1
             ? (ungetc(c, stdin), true) : false;
  }

  char readKey() {
      char c = 0;
      if (read(STDIN_FILENO, &c, 1) < 0) c = 0;
      return c;
  }
#endif

using namespace std;

// ═══════════════════════════════════════════════════════
//  Game Settings
// ═══════════════════════════════════════════════════════
const int  ROWS       = 20;
const int  COLS       = 40;
const int  BASE_SPEED = 200;   // ms per tick (lower = faster)
const char HEAD_CH    = 'O';
const char BODY_CH    = 'o';
const char FOOD_CH    = '*';
const char WALL_CH    = '#';

// ═══════════════════════════════════════════════════════
//  Types
// ═══════════════════════════════════════════════════════
struct Point { int r, c; };

enum Direction { UP, DOWN, LEFT, RIGHT };

// ═══════════════════════════════════════════════════════
//  Snake Game Class
// ═══════════════════════════════════════════════════════
class SnakeGame {
private:
    char       grid[ROWS][COLS];
    deque<Point> snake;      // snake[0] = head
    Point      food;
    Direction  dir;
    int        score;
    int        highScore;
    bool       gameOver;

    void initGrid() {
        for (int r = 0; r < ROWS; ++r)
            for (int c = 0; c < COLS; ++c) {
                if (r == 0 || r == ROWS-1 || c == 0 || c == COLS-1)
                    grid[r][c] = WALL_CH;
                else
                    grid[r][c] = ' ';
            }
    }

    void spawnFood() {
        do {
            food.r = 1 + rand() % (ROWS - 2);
            food.c = 1 + rand() % (COLS - 2);
        } while (grid[food.r][food.c] != ' ');
        grid[food.r][food.c] = FOOD_CH;
    }

    void renderGrid() {
        // draw snake on grid
        for (size_t i = 0; i < snake.size(); ++i)
            grid[snake[i].r][snake[i].c] = (i == 0) ? HEAD_CH : BODY_CH;

        // print
        system(CLEAR);
        cout << "\n  SNAKE GAME   Score: " << score
             << "   High Score: " << highScore
             << "\n  Controls: W/A/S/D or Arrow keys  |  Q = Quit\n\n";

        for (int r = 0; r < ROWS; ++r) {
            cout << "  ";
            for (int c = 0; c < COLS; ++c) {
                char ch = grid[r][c];
                if      (ch == WALL_CH) cout << "\033[1;37m" << ch << "\033[0m";
                else if (ch == HEAD_CH) cout << "\033[1;32m" << ch << "\033[0m";
                else if (ch == BODY_CH) cout << "\033[0;32m" << ch << "\033[0m";
                else if (ch == FOOD_CH) cout << "\033[1;31m" << ch << "\033[0m";
                else                    cout << ch;
            }
            cout << "\n";
        }
        cout << "\n";
    }

    void processInput() {
        if (!kbhit_ready()) return;
        char key = readKey();

        // Handle arrow keys (escape sequences on Linux)
        if (key == '\033') {
            char seq[2];
            if (read(STDIN_FILENO, seq, 2) == 2 && seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': if (dir != DOWN)  dir = UP;    break;
                    case 'B': if (dir != UP)    dir = DOWN;  break;
                    case 'C': if (dir != LEFT)  dir = RIGHT; break;
                    case 'D': if (dir != RIGHT) dir = LEFT;  break;
                }
                return;
            }
        }

        switch (tolower(key)) {
            case 'w': if (dir != DOWN)  dir = UP;    break;
            case 's': if (dir != UP)    dir = DOWN;  break;
            case 'a': if (dir != RIGHT) dir = LEFT;  break;
            case 'd': if (dir != LEFT)  dir = RIGHT; break;
            case 'q': gameOver = true;               break;
        }
    }

    void step() {
        Point head = snake.front();
        Point next = head;

        switch (dir) {
            case UP:    --next.r; break;
            case DOWN:  ++next.r; break;
            case LEFT:  --next.c; break;
            case RIGHT: ++next.c; break;
        }

        // wall collision
        if (grid[next.r][next.c] == WALL_CH) { gameOver = true; return; }

        // self collision
        for (size_t i = 1; i < snake.size(); ++i)
            if (snake[i].r == next.r && snake[i].c == next.c)
                { gameOver = true; return; }

        bool ate = (grid[next.r][next.c] == FOOD_CH);

        // clear tail on grid (before moving)
        Point tail = snake.back();
        grid[tail.r][tail.c] = ' ';

        // move snake
        snake.push_front(next);
        if (!ate) {
            snake.pop_back();
        } else {
            score += 10;
            if (score > highScore) highScore = score;
            spawnFood();
        }
    }

public:
    SnakeGame() : dir(RIGHT), score(0), highScore(0), gameOver(false) {
        srand((unsigned)time(nullptr));
    }

    void run() {
        char replay;
        do {
            // reset
            initGrid();
            snake.clear();
            score    = 0;
            gameOver = false;
            dir      = RIGHT;

            // initial snake (length 3) in the middle
            int mr = ROWS / 2, mc = COLS / 2;
            snake.push_back({mr, mc});
            snake.push_back({mr, mc - 1});
            snake.push_back({mr, mc - 2});

            spawnFood();

#ifndef _WIN32
            setRawMode(true);
#endif
            while (!gameOver) {
                processInput();
                step();
                if (!gameOver) renderGrid();
                SLEEP(BASE_SPEED);
            }
#ifndef _WIN32
            setRawMode(false);
#endif
            system(CLEAR);
            cout << "\n  ╔══════════════════════════════╗\n";
            cout << "  ║         GAME  OVER           ║\n";
            cout << "  ╠══════════════════════════════╣\n";
            cout << "  ║  Score      : " << left << setw(14) << score      << "║\n";
            cout << "  ║  High Score : " << left << setw(14) << highScore  << "║\n";
            cout << "  ╚══════════════════════════════╝\n\n";

            cout << "  Play again? (y/n): ";
            cin  >> replay;

        } while (tolower(replay) == 'y');

        cout << "\n  Thanks for playing Snake!\n\n";
    }
};

// ═══════════════════════════════════════════════════════
//  Entry Point
// ═══════════════════════════════════════════════════════
int main() {
    SnakeGame game;
    game.run();
    return 0;
}
