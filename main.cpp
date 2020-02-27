#include <iostream>
#include <ncurses.h>

int main()
{
    initscr();
    printw("Hello!");
    getch();
    endwin();

    return 0;
}