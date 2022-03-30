#ifndef __SNAKE_H__
#define __SNAKE_H__

#include "winsys.h"
#include <vector>

class CSnake:public CFramedWindow
{
private:
    struct Item : public CPoint {
        char sign;
        Item(int x = 1, int y = 1, char s = 'O') : CPoint(x, y), sign(s) {}
    };
    enum
    {
        PAUSE,
        RUN,
        HELP,
        OVER
    } state;
    Item apple;
    vector<Item> snake;
    int dir{};
    int score{};
    int delay;
    int speed;

    void generateApple();
    void restart();
    void move (const CPoint & delta);
    void otherSide();
    void eatApple(Item &tail);

    void paintPause();
    void paintHelp();
    void paintOver();
    void paintSnake();
    void clearWin();

public:
    CSnake(CRect r, char _c = ' ');
    bool handleEvent(int key) override;
    void paint() override;
};

#endif
