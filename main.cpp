#include <iostream>
#include <ncurses.h>

void draw_crossing(WINDOW* win, int lanes, int slots)
{
    int side_length = 2*slots + lanes;
    //drawing horizontal lines
    for(int i = 0; i < lanes; ++i ){
        for(int j = 1; j <= side_length; ++j){
            mvwprintw(win, slots+i+1, j, ".");
        }
    }

    //drawing vertical lines
    for(int i = 0; i < lanes; ++i ){
        for(int j = 1; j <= side_length; ++j){
            mvwprintw(win, j, slots+i+1, ".");
        }
    }
}

int main(int argc, char* argv[])
{
    int lanes, slots;
    lanes = atoi(argv[1]);
    slots = atoi(argv[2]);
    int height, width, start_y, start_x;
    height = width = 2*slots + lanes;
    start_y = start_x = 1;

    initscr();//initializes the screen and sets up memory and clears the screen

    WINDOW* win = newwin(height+2, width+2, start_y, start_x);
    refresh();
    box(win, 0, 0);
  //  wprintw(win, "Traffic Lights");
    wrefresh(win);
    draw_crossing(win, lanes,slots);
    wrefresh(win);
    getch();

    endwin();//deallocates memory and ends ncurses

    return 0;
}