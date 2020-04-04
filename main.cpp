#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include <atomic>
#include "common.h"
#include "Emergency.cpp"

std::mutex mtx;
std::vector<std::vector<bool>> OCCUPIED_POSITIONS;
WINDOW* init_map(RoadState &road_state)
{
    int y_max_size, x_max_size;

    initscr();
    getmaxyx(stdscr, y_max_size, x_max_size);
    cbreak();
    curs_set(0);
    WINDOW* win = newwin(road_state.wall+2,
                        road_state.wall+2,
                        y_max_size/2-road_state.wall/2,
                        x_max_size/2-road_state.wall/2);
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
            OCCUPIED_POSITIONS[j][road_state.slots+i+1] = false;
        }
    }

    //vertical
    for(int i = 0; i < road_state.lanes;++i){
        for(int j=1;j < road_state.wall+1;++j){
            mvwprintw(win, j, road_state.slots+i+1, ".");
            OCCUPIED_POSITIONS[road_state.slots+i+1][j] = false;
        }
    }
    wrefresh(win);
}

void draw_E(WINDOW* win,Emergency* karetka, Movement_direction where)
{ 
    karetka->calculate_movement_to_do(where);

    mtx.lock();
    karetka->set_on_junction(karetka->start_pos); 
    mtx.unlock();

    while (!karetka->getHasArrived())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        mtx.lock();
        karetka->move(where);
        mtx.unlock();
    }
    mtx.lock();
    karetka->~Emergency();
    mtx.unlock();
}

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cout << "Niepoprawna liczba argumentow!\nNalezy podac dwa argumenty"<<std::endl;
        return 0;
    }
    WINDOW* win;

    RoadState road_state(atoi(argv[1]), atoi(argv[2]));

    win = init_map(road_state);
    draw_map(win, road_state);

    std::thread input(read_input);

    Emergency* karetkaR = new Emergency(win, road_state, RIGHT);
    Emergency* karetkaL = new Emergency(win, road_state, LEFT);
    Emergency* karetkaT = new Emergency(win, road_state, TOP);
    Emergency* karetkaB = new Emergency(win, road_state, BOT);
    Emergency* karetkaR2 = new Emergency(win, road_state, RIGHT);
    std::thread moveER_R(draw_E, win, karetkaR, TURN_RIGHT);
    std::thread moveER_L(draw_E, win, karetkaL, TURN_RIGHT);
    std::thread moveER_T(draw_E, win, karetkaT, TURN_RIGHT);
    std::thread moveER_B(draw_E, win, karetkaB, TURN_RIGHT);
    // std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::thread moveER_R2(draw_E, win, karetkaR2, TURN_RIGHT);
    moveER_R.join();
    moveER_L.join();
    moveER_T.join();
    moveER_B.join();
    moveER_R2.join();
    input.join();

    return 0;
}