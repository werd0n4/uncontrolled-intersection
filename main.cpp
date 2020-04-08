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

void read_input(std::vector<Car*>* cars)
{
    bool quit = false;
    while (!quit)
    {
        switch(std::cin.get()){
            case 189://MINUS - slow down cars
                for(auto& car : *cars){
                    car->slowDown();
                }
                break;

            case 27://ESC - exit program
                endwin();
                quit = true;
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

void draw_E(WINDOW* win,Car* car, Movement_direction where)
{
    car->calculate_movement_to_do(where);

    while(true){//Setting vehicle on the road
        mtx.lock();
        bool isStartPositionOccupied = car->isStartPositionOccupied();
        mtx.unlock();
        if(!isStartPositionOccupied){
            mtx.lock();
            car->set_on_junction();
            mtx.unlock();
            break;
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(car->getDefaultSpeed()));
        }
    } 
    //car is driving
    while (!car->getHasArrived())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(car->getDefaultSpeed()));
        mtx.lock();
        car->erase_last_position();
        mtx.unlock();

        car->calculate_next_position(where);

        while (car->road_state->OCCUPIED_POSITIONS[car->position.second][car->position.first])
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        mtx.lock();
        car->move();
        mtx.unlock();
    }
    mtx.lock();
    car->~Car();
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

    std::vector<Car*> cars;
    // Car* carR = new Car(win, road_state, RIGHT);
    Car* carL = new Car(win, road_state, LEFT);
    Car* carT = new Car(win, road_state, TOP);
    // Car* carB = new Car(win, road_state, BOT);
    Car* carR2 = new Car(win, road_state, RIGHT);
    // Car* carT2 = new Car(win, road_state, TOP);
    // std::thread moveER_R(draw_E, win, carR, FORWARD);
    
    cars.push_back(carL);
    cars.push_back(carT);
    std::thread input(read_input, &cars);
    std::thread moveER_T(draw_E, win, carT, FORWARD);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::thread moveER_L(draw_E, win, carL, FORWARD);
    // std::thread moveER_B(draw_E, win, carB, TURN_RIGHT);
    std::thread moveER_R2(draw_E, win, carR2, TURN_RIGHT);
    // std::thread moveER_T2(draw_E, win, carT2, FORWARD);
    // moveER_R.join();
    moveER_L.join();
    moveER_T.join();
    // moveER_B.join();
    moveER_R2.join();
    // moveER_T2.join();
    input.join();

    return 0;
}