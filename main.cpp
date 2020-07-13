#include <iostream>
#include <ncurses.h>
#include <thread>
#include <mutex>
#include <memory>
#include <time.h>

#include "Car.cpp"

bool running;
bool isPaused;
WINDOW* win;
RoadState road_state(2,10);

void init_map()
{
    int y_max_size, x_max_size;

    initscr();

    start_color();
    //Color means car destination
    init_pair(1, COLOR_GREEN, COLOR_BLACK);// TOP
    init_pair(2, COLOR_RED, COLOR_BLACK);// Right
    init_pair(3, COLOR_CYAN, COLOR_BLACK);// BOT
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);// LEFT
    
    getmaxyx(stdscr, y_max_size, x_max_size);
    curs_set(0);

    win = newwin(road_state.wall+2, road_state.wall+2, y_max_size/2-road_state.wall/2, x_max_size/2-road_state.wall/2);

    WINDOW* legend = newwin(20, 100, 0, 0);
    mvwprintw(legend, 1, 1, "Each car has a color which indicates its\n destination (from OBSERVATOR'S PERSPECTIVE):");
    
    wattron(legend, COLOR_PAIR(1));
    mvwprintw(legend, 4, 1, "- Green - TOP");
    wattroff(legend, COLOR_PAIR(1));

    wattron(legend, COLOR_PAIR(2));
    mvwprintw(legend, 5, 1, "- Red - RIGHT");
    wattroff(legend, COLOR_PAIR(2));

    wattron(legend, COLOR_PAIR(3));
    mvwprintw(legend, 6, 1, "- Light blue - BOT");
    wattroff(legend, COLOR_PAIR(3));

    wattron(legend, COLOR_PAIR(4));
    mvwprintw(legend, 7, 1, "- Orange - LEFT");
    wattroff(legend, COLOR_PAIR(4));

    mvwprintw(legend, 9, 1, "Shortcuts:\n- ESC - exit program\n- P - pause progra");

    wrefresh(legend);
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
                std::cout << std::endl;
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
    // werase(win);
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

void refresh_screen(std::vector<Car>& cars)
{
    std::pair<int, int> position;
    while(running)
    {
        draw_map();
        for(auto& car : cars)
        {
            //rysowanie aut
            position = car.getPosition();
            if(!car.getHasArrived()){
                wattron(win,COLOR_PAIR(car.destination+1));
                mvwprintw(win, position.first, position.second, car.getSymbol());
                wattroff(win,COLOR_PAIR(car.destination+1));
            }
            wrefresh(win);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, char* argv[])
{

    isPaused = false;
    running = true;
    srand (time(NULL));
    int carNumber = atoi(argv[1]);
    carNumber = carNumber%27;
    if(carNumber == 0)
        carNumber = 1;
    char c[26][2] = {"A", "B", "C", "D", "E",
                     "F", "G", "H", "I", "J",
                     "K", "L", "M", "N", "O",
                     "P", "Q", "R", "S", "T",
                     "U", "V", "W", "X", "Y", "Z"};


    std::vector<Car> cars;
    std::vector<std::thread> carThreads;

    init_map();

    for(int i = 0; i < carNumber; ++i){
        cars.push_back(Car(win, road_state, Road_Pos(rand()%4), Movement_direction(rand()%3), c[i]));
    }

    std::thread input([]{read_input();});
    std::thread screenRefresh([&cars]{refresh_screen(cars);});

    for(auto& car : cars)
    {
        carThreads.push_back(std::thread([&car]{car.ride();}));
    }

    for(auto& thread : carThreads)
    {
        thread.join();
    }
    screenRefresh.join();
    input.join();

    return 0;
}