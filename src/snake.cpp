#include "snake.h"
#include "screen.h"

//CSnake::CSnake(CRect r, char _c /*=' '*/):
//  CFramedWindow(r, _c)
//{
//}


//struct CSnake::Item : public CPoint {
//    char sign;
//    Item(int x, int y, char s) : CPoint(x, y), sign(s) {}
//    Item() : CPoint(0,0), sign(' ') {}
//};

CSnake::CSnake(CRect r, char _c) : CFramedWindow(r, _c)
{
    srand(time(nullptr));
    restart();
}

void CSnake::generateApple()
{
    bool empty;
    do {
        apple.x = rand() % (geom.size.x - 2) + 1;
        apple.y = rand() % (geom.size.y - 2) + 1;

        empty = true;
        for (auto &item : snake)
        {
            if (item.x == apple.x && item.y == apple.y)
            {
                empty = false;
                break;
            }
        }
    } while(!empty);
}

void CSnake::restart()
{
    int x = 7, y = 3;
    snake.clear();
    snake.push_back(Item(x, y, '*'));
    snake.push_back(Item(x - 1, y, '+'));
    snake.push_back(Item(x - 2, y, '+'));

    state = HELP;
    generateApple();
    dir = KEY_RIGHT;
    score = 0;
    delay = 0;
    speed = 28;
}

void CSnake::move(const CPoint &delta)
{
    if (state != RUN)
        return CFramedWindow::move(delta);

    Item tail = snake.back();
    for (size_t i = snake.size() - 1; i > 0; i--)
    {
        snake[i].x = snake[i-1].x;
        snake[i].y = snake[i-1].y;
    }

    snake[0] += delta;
    for (size_t i = 1; i < snake.size(); i++)
    {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
        {
            state = OVER;
            return;
        }
    }

    otherSide();
    eatApple(tail);
}

void CSnake::otherSide()
{
    if (snake[0].x == geom.size.x - 1)
        snake[0].x = 1;
    if (snake[0].x == 0)
        snake[0].x = geom.size.x - 2;

    if (snake[0].y == geom.size.y - 1)
        snake[0].y = 1;
    if (snake[0].y == 0)
        snake[0].y = geom.size.y - 2;
}

void CSnake::eatApple(Item &tail)
{
    if (snake[0].x == apple.x && snake[0].y == apple.y)
    {
        score++;
        if (speed > 22) speed -= 3;
        else if (speed > 16) speed -= 2;
        else if (speed > 5) speed -= 1;
        snake.push_back(tail);
        generateApple();
    }
}

void CSnake::paintPause()
{
    int x = geom.topleft.x + geom.size.x / 2 - 3;
    int y = geom.topleft.y + geom.size.y / 2;
    gotoyx(y, x);
    printl("PAUSED");
}
void CSnake::paintHelp()
{
    int x = geom.topleft.x + 3;
    int y = geom.topleft.y + 4;

    const size_t size = 6;
    const size_t max_line_size = 40;
    char help[size][max_line_size] = {
            "h - toggle help information",
            "p - toggle play/pause mode",
            "r - restart game",
            "q - quit",
            "arrows - move snake (in play move) or",
            "move window (in pause mode)"
    };

    for (size_t i = 0; i < size; i++)
    {
        if (i == size - 1) x += 9;
        gotoyx(y++, x);
        printl(help[i]);
    }

}
void CSnake::paintOver()
{
    int x = geom.topleft.x + 1;
    int y = geom.topleft.y + 1;
    gotoyx(y, x);
    printl("GAME OVER, result: %d", score);
}

void CSnake::paintSnake()
{
    clearWin();
    for (auto & i : snake)
    {
        gotoyx(geom.topleft.y + i.y, geom.topleft.x + i.x);
        printc(i.sign);
    }

    gotoyx(geom.topleft.y + apple.y, geom.topleft.x + apple.x);
    printc(apple.sign);
}

void CSnake::clearWin()
{
    for (int y = 1; y < geom.size.y - 1; y++)
    {
        for (int x = 1; x < geom.size.x - 1; x++)
        {
            gotoyx(geom.topleft.y + y, geom.topleft.x + x);
            printc(' ');
        }
    }
}

bool CSnake::handleEvent(int key)
{
    /*
     * pause
     * help
     * restart
     * direction
     */

    switch (key) {
        case 'P':
        case 'p':
            if (state == OVER) restart();
            else state = (state == PAUSE ||
                    state == HELP
                    ? RUN : PAUSE);
            return true;
        case 'H':
        case 'h':
            if (state == OVER) restart();
            else state = (state == HELP ? RUN : HELP);
            return true;
        case 'R':
        case 'r':
            restart();
            return true;
        default:
            if (state != RUN)
                return CFramedWindow::handleEvent(key);
    }

    int x_def = (int)(dir == KEY_RIGHT) - (int)(dir == KEY_LEFT);
    int y_def = (int)(dir == KEY_DOWN) - (int)(dir == KEY_UP);
    CPoint delta = CPoint(x_def, y_def);

    switch (key) {
        case KEY_UP:
            if (dir != KEY_DOWN) {
                delta = CPoint(0, -1);
                dir = KEY_UP;
            }
            break;
        case KEY_DOWN:
            if (dir != KEY_UP) {
                delta = CPoint(0, 1);
                dir = KEY_DOWN;
            }
            break;
        case KEY_RIGHT:
            if (dir != KEY_LEFT) {
                delta = CPoint(1, 0);
                dir = KEY_RIGHT;
            }
            break;
        case KEY_LEFT:
            if (dir != KEY_RIGHT) {
                delta = CPoint(-1, 0);
                dir = KEY_LEFT;
            }
            break;
    };
    delay = (delay + 1) % speed;
    if (delay == 0)
        move(delta);

    return true;
}
void CSnake::paint()
{
    CFramedWindow::paint();
    /*
     * score
     * help/pause/over
     */
    gotoyx(geom.topleft.y - 1, geom.topleft.x);
    printl("| SCORE: %d |", score);

    switch (state) {
        case RUN:
            paintSnake();
            break;
        case HELP:
            paintHelp();
            break;
        case PAUSE:
            paintPause();
            break;
        case OVER:
            paintSnake();
            paintOver();
            break;
    }
}
