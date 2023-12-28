#include <cstdlib>
#include <ncurses.h>
#include <deque>
#include <vector>
#include <unistd.h>

bool GameOver;
const int WIDTH = 20;
const int HEIGHT = 20;
int x, y;
int fruitX, fruitY;
int score;

struct Field {
    int width;
    int height;
    std::vector<std::vector<char>> cells;

    Field(int w, int h) : width(w), height(h), cells(h + 1, std::vector<char>(w + 1, ' ')) {}

    void clear() {
        for (int i = 0; i <= height; i++) {
            for (int j = 0; j <= width; j++) {
                cells[i][j] = ' ';
            }
        }
    }

    void setCell(int x, int y, char value) {
        cells[y][x] = value;
    }

    char getCell(int x, int y) const {
        return cells[y][x];
    }
};

typedef enum DIRECTION { STOP = 0, LEFT, RIGHT, UP, DOWN } Dir;
Dir dir;

std::deque<std::pair<int, int>> snake;

struct Apple {
    int x, y;
};

Field field(WIDTH, HEIGHT);

void Initialize()
{
    initscr();
    clear();
    noecho();
    cbreak();
    curs_set(0);

    GameOver = false;
    dir = STOP;
    x = WIDTH / 2;
    y = HEIGHT / 2;

    field.clear();

    snake.push_front(std::make_pair(x, y));

    fruitX = (rand() % WIDTH) + 1;
    fruitY = (rand() % HEIGHT) + 1;

    score = 0;
}

void Render()
{
    clear();
    for (int i = 0; i <= HEIGHT; i++) {
        for (int j = 0; j <= WIDTH; j++) {
            if (i == 0 || i == HEIGHT) {
                mvprintw(i, j, ".");
            } else if (j == 0 || j == WIDTH) {
                mvprintw(i, j, ".");
            } else {
                mvprintw(i, j, "%c", field.getCell(j, i));
            }
        }
    }

    mvprintw(fruitY, fruitX, "@");

    mvprintw(16, 29, "Score = %d", score);
    refresh();
}

void Poll_Input()
{
    keypad(stdscr, TRUE);
    halfdelay(1);

    int c = getch();

    switch (c) {
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        case 27:
            GameOver = true;
            break;
    }
}

void Update()
{
    int new_x = snake.front().first;
    int new_y = snake.front().second;

    switch (dir) {
        case LEFT:
            new_x--;
            break;
        case RIGHT:
            new_x++;
            break;
        case UP:
            new_y--;
            break;
        case DOWN:
            new_y++;
            break;
        default:
            break;
    }

    if (new_x >= WIDTH || new_x < 1 || new_y < 1 || new_y >= HEIGHT) {
        GameOver = true;
    }

    if (new_x == fruitX && new_y == fruitY) {
        score++;
        fruitX = (rand() % WIDTH) + 1;
        fruitY = (rand() % HEIGHT) + 1;
    } else {
        field.setCell(snake.back().first, snake.back().second, ' ');
        snake.pop_back();
    }

    for (const auto& segment : snake) {
        if (segment.first == new_x && segment.second == new_y) {
            GameOver = true;
        }
    }

    snake.push_front(std::make_pair(new_x, new_y));

    field.clear();

    for (const auto& segment : snake) {
        field.setCell(segment.first, segment.second, 'o');
    }

    field.setCell(fruitX, fruitY, '@');
}

int main(void)
{
    Initialize();

    while (!GameOver) {
        Render();
        Poll_Input();
        Update();
        
        usleep(80000);
    }

    getch();
    endwin();

    return 0;
}
//g++ -std=c++11 snake.cpp -o snake -lncurses
