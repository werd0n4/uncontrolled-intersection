#include <iostream>
#include <ncurses.h>
#include <thread>


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

void draw_map(int lanes, int slots)
{
    int wall, start_y, start_x;
    wall = 2*slots + lanes;
    start_y = start_x = 1;

    initscr();//initializes the screen and sets up memory and clears the screen

    WINDOW* win = newwin(wall+2, wall+2, start_y, start_x);
    box(win, 0, 0);
    draw_crossing(win, lanes,slots);
    curs_set(0);

    while(true) 
    {
        wrefresh(win);
        if(std::cin.get() == 27)
            break;
    }

    endwin();//deallocates memory and ends ncurses
}

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cout << "Niepoprawna liczba argumentow!\nNalezy podac dwa argumenty"<<std::endl;
        return 0;
    }

    std::thread map(draw_map, atoi(argv[1]), atoi(argv[2]));
    map.join();

    return 0;
}