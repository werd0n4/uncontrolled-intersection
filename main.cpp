#include <iostream>
#include <ncurses.h>
#include <string.h>

void draw_crossing(int lanes, int slots)
{
    int dimensions = 0;
    dimensions = getmaxy(stdscr);
    
    //horizontal lines
    for(int i = 0; i<lanes; ++i){
        for(int j = 0; j<2*slots+lanes; j++){
            mvaddch(dimensions/2+i, j +dimensions/2,'.');
        };
    }
    //vertical lines
    for(int i = 0; i<lanes; ++i){
        for(int j = 0; j<2*slots+lanes; j++){
            mvaddch(j+dimensions/2-slots, i+slots +dimensions/2, '.');
        };
    }

}

int main()
{
    initscr();//initializes the screen and sets up memory and clears the screen
    draw_crossing(4,10);
    getch();

    endwin();//deallocates memory and ends ncurses

    return 0;
}