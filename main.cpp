#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include <memory>
#include "Car.cpp"

std::mutex mtx;
bool running = true;
bool isPaused = false;
WINDOW* win;
WINDOW* outside;
RoadState* road_state;

WINDOW* init_map()
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
    bool quit = false;
    while (!quit)
    {
        switch(std::cin.get()){
            case 27://ESC - exit program
                endwin();
                std::cout << "Uncontrolled intersection simulator v1.0" << std::endl;
                std::cout << "Filip Gajewski i Tymoteusz Frankiewicz 2020" << std::endl;
                std::cout << std::endl;
                std::cout << "Drive save and focus on the road :D" << std::endl;
                quit = true;
                break;
            case 'p'://p - pause program
                isPaused = !isPaused;
                break;
        }
    }
    exit(0);
}

void draw_map()
{
    clear();
    box(win,0,0);

    //horizontal
    for(int i=0;i<road_state->lanes;++i){
        for(int j=1;j<road_state->wall+1;++j){
            mvwprintw(win, road_state->slots+i+1,j , ".");
        //  road_state->OCCUPIED_POSITIONS[i][road_state->slots+j+1] = false;
        }
    }

    //vertical
    for(int i = 0; i < road_state->lanes;++i){
        for(int j=1;j < road_state->wall+1;++j){
           mvwprintw(win, j, road_state->slots+i+1, ".");
//           road_state->OCCUPIED_POSITIONS[road_state->slots+i+1][j] = false;
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
        bool isStartPositionOccupied = car.isStartPositionOccupied();
        if(!isStartPositionOccupied){
            car.set_on_junction();
            carIsSet = true;
        }
        else{
            std::this_thread::sleep_for(std::chrono::milliseconds(car.getSpeed()));
        }
    } 
    //car is driving
    while (!car.getHasArrived())
    {
        while(!isPaused){
            std::this_thread::sleep_for(std::chrono::milliseconds(car.getSpeed()));
            car.move(where);
        }
    }
    // car.~Car();
}

int main(int argc, char* argv[])
{
    // if(argc != 3){
    //     // std::cout << "Niepoprawna liczba argumentow!\nNalezy podac dwa argumenty"<<std::endl;
    //     argv[1] = (char*)"2";
    //     argv[2] = (char*)"8";
    //     // return 0;
    // }

    std::vector<Car> cars;
    std::vector<std::thread> carThreads;
    // road_state = new RoadState(atoi(argv[1]), atoi(argv[2]));
    road_state = new RoadState(2,8);

    win = init_map();
    //test data
    cars.push_back(Car(win, road_state, TOP, (char*)"A"));
    cars.push_back(Car(win, road_state, TOP, (char*)"B"));
    cars.push_back(Car(win, road_state, RIGHT, (char*)"C"));
    cars.push_back(Car(win, road_state, RIGHT, (char*)"D"));
    cars.push_back(Car(win, road_state, RIGHT, (char*)"X"));
    cars.push_back(Car(win, road_state, BOT, (char*)"E"));
    cars.push_back(Car(win, road_state, BOT, (char*)"F"));
    cars.push_back(Car(win, road_state, LEFT, (char*)"G"));
    cars.push_back(Car(win, road_state, LEFT, (char*)"H"));

    std::thread input([](){read_input();});
    std::thread screenRefresh([&cars](){refreshScreen(cars);});

    for(auto it = cars.begin(); it != cars.end(); ++it)
    {
        carThreads.push_back(std::thread([it](){draw_Car(*it, FORWARD);}));
    }

    for(auto it = carThreads.begin(); it != carThreads.end(); ++it)
    {
        (*it).join();
    }
    screenRefresh.join();
    input.join();


    return 0;
}