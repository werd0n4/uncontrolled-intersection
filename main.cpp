#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include <memory>
#include <time.h>

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

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK);// Forward
    init_pair(2, COLOR_RED, COLOR_BLACK);// Right
    init_pair(3, COLOR_CYAN, COLOR_BLACK);// Left

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
        }
    }

    //vertical
    for(int i = 0; i < road_state->lanes;++i){
        for(int j=1;j < road_state->wall+1;++j){
           mvwprintw(win, j, road_state->slots+i+1, ".");
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
                wattron(win,COLOR_PAIR((*it).dir+1));
                mvwprintw(win, position.first, position.second, (*it).getSymbol());
                wattroff(win,COLOR_PAIR((*it).dir+1));
            }
            wrefresh(win);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void draw_Car(Car& car)
{
    car.calculate_movement_to_do();
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
            car.move();
        }
    }
    // car.~Car();
}

int main(int argc, char* argv[])
{
    srand (time(NULL));
    int carNumber = atoi(argv[1]);
    carNumber = carNumber%26;
    if(carNumber == 0)
        carNumber = 1;
    char c[26][2] = {"A", "B", "C", "D", "E",
                     "F", "G", "H", "I", "J",
                     "K", "L", "M", "N", "O",
                     "P", "Q", "R", "S", "T",
                     "U", "V", "W", "X", "Y", "Z"};


    std::vector<Car> cars;
    std::vector<std::thread> carThreads;
    road_state = new RoadState(2,8);

    win = init_map();

    for(int i = 0; i < carNumber; ++i){
        cars.push_back(Car(win, road_state, Road_Pos(rand()%4), Movement_direction(rand()%3), c[i]));
    }

    std::thread input([](){read_input();});
    std::thread screenRefresh([&cars](){refreshScreen(cars);});

    for(auto it = cars.begin(); it != cars.end(); ++it)
    {
        carThreads.push_back(std::thread([it](){draw_Car(*it);}));
    }

    for(auto it = carThreads.begin(); it != carThreads.end(); ++it)
    {
        (*it).join();
    }
    screenRefresh.join();
    input.join();


    return 0;
}