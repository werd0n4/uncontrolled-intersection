#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>

#include "Emergency.cpp"

//int lanes, slots, wall;
char*** characters;
Emergency* karetka;
std::mutex map_mutex;

WINDOW* init_map(RoadState &road_state)
{
    //characters = new char**[wall];
    // for(int i=0;i<wall;i++){
    //     characters[i]=new char*[wall];
    // }

    // for(int i=0;i<wall;++i){
    //     for(int j=0;j<wall;++j){
    //         characters[i][j]=(char*)".";
    //     }
    // }
    initscr();
    cbreak();
    curs_set(0);
    WINDOW* win = newwin(road_state.wall+2,road_state.wall+2,0,0);
    return win;
}

void read_input()
{
    while (true)
    {
        if(std::cin.get() == 27){
            endwin();
            break;
        }
    }
    exit(0);
}

void draw_map(WINDOW* win, RoadState& road_state)
{
    box(win,0,0);
    //horizontal
    for(int i=0;i<road_state.lanes;++i){
        for(int j=1;j<road_state.wall+1;++j){
            mvwprintw(win, road_state.slots+i+1,j , ".");
        }
    }
    //vertical
    for(int i = 0; i < road_state.lanes;++i){
        for(int j=1;j < road_state.wall+1;++j){
            mvwprintw(win, j, road_state.slots+i+1, ".");
        }
    }
    wrefresh(win);
}

void draw_E(WINDOW* win)
{
    karetka->set_on_junction(2);
    wrefresh(win);
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        karetka->move();
        std::lock_guard<std::mutex> guard(map_mutex);
        wrefresh(win);
    }
}

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cout << "Niepoprawna liczba argumentow!\nNalezy podac dwa argumenty"<<std::endl;
        return 0;
    }
    WINDOW* win;
    RoadState road_state(atoi(argv[1]), atoi(argv[2]));

    //lanes = atoi(argv[1]);
    //slots = atoi(argv[2]);
    //wall = 2 * slots + lanes;

    win = init_map(road_state);
    draw_map(win, road_state);
    karetka = new Emergency(win, road_state, 1);//!!!!!!!!!

    std::thread input(read_input);
    std::thread moveER(draw_E,win);
    input.join();
    moveER.join();

    return 0;
}