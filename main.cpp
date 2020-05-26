#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include <memory>
#include "Car.cpp"

std::mutex mtx;
bool running = true;
WINDOW* win;
RoadState road_state;

WINDOW* init_map()
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
    bool quit = false;
    while (!quit)
    {
        switch(std::cin.get()){
            case 27://ESC - exit program
                endwin();
                quit = true;
                break;
        }
    }
    exit(0);
}

void draw_map()
{
    box(win,0,0);
    //horizontal
    for(int i=0;i<road_state.lanes;++i){
        for(int j=1;j<road_state.wall+1;++j){
            mvwprintw(win, road_state.slots+i+1,j , ".");
            road_state.OCCUPIED_POSITIONS[i][road_state.slots+j+1] = false;
        }
    }

    //vertical
    for(int i = 0; i < road_state.lanes;++i){
        for(int j=1;j < road_state.wall+1;++j){
            mvwprintw(win, j, road_state.slots+i+1, ".");
            road_state.OCCUPIED_POSITIONS[road_state.slots+i+1][j] = false;
        }
    }
    wrefresh(win);
}

void refreshScreen(std::vector<Car>& cars)
{
    std::pair<int, int> position;
    while(running)
    {
        draw_map();
        for(auto it = cars.begin(); it != cars.end(); ++it)
        {
            //rysowanie aut
            position = (*it).getPosition();
            if(!(*it).getHasArrived()){
                mvwprintw(win, position.first, position.second, (*it).getSymbol());
            }
            // else{
            //     cars.erase(it--);
            //     mvwprintw(win, position.first, position.second, ".");
            // }
            wrefresh(win);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void draw_Car(Car& car, Movement_direction where)
{
    car.calculate_movement_to_do(where);
    bool carIsSet = false;

    while(!carIsSet){//Setting vehicle on the road
        mtx.lock();
        bool isStartPositionOccupied = car.isStartPositionOccupied();
        mtx.unlock();
        if(!isStartPositionOccupied){
            mtx.lock();
            car.set_on_junction();
            mtx.unlock();
            carIsSet = true;
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(car.getSpeed()));
        }
    } 
    //car is driving
    while (!car.getHasArrived())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(car.getSpeed()));
        mtx.lock();
        car.erase_last_position();
        mtx.unlock();

        car.calculate_next_position(where);
        
        while (car.road_state.OCCUPIED_POSITIONS[car.position.second][car.position.first])
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        mtx.lock();
        car.move();
        mtx.unlock();
    }
    mtx.lock();
    car.~Car();
    mtx.unlock();
}

int main(int argc, char* argv[])
{
    if(argc != 3){
        std::cout << "Niepoprawna liczba argumentow!\nNalezy podac dwa argumenty"<<std::endl;
        return 0;
    }

    std::vector<Car> cars;
    std::vector<std::thread> carThreads;
    road_state = RoadState(atoi(argv[1]), atoi(argv[2]));

    win = init_map();

    cars.push_back(Car(win, road_state, LEFT, "A"));
    cars.push_back(Car(win, road_state, LEFT, "E"));
    cars.push_back(Car(win, road_state, TOP, "B"));
    cars.push_back(Car(win, road_state, RIGHT, "C"));
    cars.push_back(Car(win, road_state, RIGHT, "D"));

    std::thread input([](){read_input();});

    for(auto it = cars.begin(); it != cars.end(); ++it)
    {
        carThreads.push_back(std::thread([it](){draw_Car(*it, FORWARD);}));
    }

    std::thread screenRefresh([&cars](){refreshScreen(cars);});

    for(auto it = carThreads.begin(); it != carThreads.end(); ++it)
    {
        (*it).join();
    }
    screenRefresh.join();
    input.join();

    return 0;
}