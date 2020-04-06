#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include "Car.cpp"

std::mutex mtx;

WINDOW* init_map(RoadState* road_state)
{
    int y_max_size, x_max_size;

    initscr();
    getmaxyx(stdscr, y_max_size, x_max_size);
    cbreak();
    curs_set(0);
    WINDOW* win = newwin(road_state->wall+2,
                        road_state->wall+2,
                        y_max_size/2-road_state->wall/2,
                        x_max_size/2-road_state->wall/2);
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

void draw_map(WINDOW* win, RoadState* road_state)
{

    box(win,0,0);
    //horizontal
    for(int i=0;i<road_state->lanes;++i){
        for(int j=1;j<road_state->wall+1;++j){
            mvwprintw(win, road_state->slots+i+1,j , ".");
            road_state->OCCUPIED_POSITIONS[j][road_state->slots+i+1] = false;
        }
    }

    //vertical
    for(int i = 0; i < road_state->lanes;++i){
        for(int j=1;j < road_state->wall+1;++j){
            mvwprintw(win, j, road_state->slots+i+1, ".");
            road_state->OCCUPIED_POSITIONS[road_state->slots+i+1][j] = false;
        }
    }
    wrefresh(win);
}

void draw_E(WINDOW* win,Car* karetka, Movement_direction where)
{
    karetka->calculate_movement_to_do(where);

    while(true){//Setting vehicle on the road
        mtx.lock();
        bool isStartPositionOccupied = karetka->isStartPositionOccupied();
        mtx.unlock();
        if(!isStartPositionOccupied){
            mtx.lock();
            karetka->set_on_junction();
            mtx.unlock();
            break;
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(karetka->getDefaultSpeed()));
        }
    } 
    //car is driving
    while (!karetka->getHasArrived())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(karetka->getDefaultSpeed()));
        mtx.lock();
        karetka->move(where);
        mtx.unlock();
    }
    mtx.lock();
    karetka->~Car();
    mtx.unlock();
}

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cout << "Niepoprawna liczba argumentow!\nNalezy podac dwa argumenty"<<std::endl;
        return 0;
    }
    WINDOW* win;

    RoadState* road_state = new RoadState(atoi(argv[1]), atoi(argv[2]));

    win = init_map(road_state);
    draw_map(win, road_state);

    std::thread input(read_input);

    // Car* karetkaR = new Car(win, road_state, RIGHT);
    Car* karetkaL = new Car(win, road_state, LEFT);
    Car* karetkaT = new Car(win, road_state, TOP);
    // Car* karetkaB = new Car(win, road_state, BOT);
    // Car* karetkaR2 = new Car(win, road_state, RIGHT);
    // Car* karetkaT2 = new Car(win, road_state, TOP);
    // std::thread moveER_R(draw_E, win, karetkaR, FORWARD);
    std::thread moveER_T(draw_E, win, karetkaT, FORWARD);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread moveER_L(draw_E, win, karetkaL, FORWARD);
    // std::thread moveER_B(draw_E, win, karetkaB, TURN_RIGHT);
    // std::thread moveER_R2(draw_E, win, karetkaR2, FORWARD);
    // std::thread moveER_T2(draw_E, win, karetkaT2, FORWARD);
    // moveER_R.join();
    moveER_L.join();
    moveER_T.join();
    // moveER_B.join();
    // moveER_R2.join();
    // moveER_T2.join();
    input.join();

    return 0;
}