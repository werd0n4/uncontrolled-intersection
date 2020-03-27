#include <ncurses.h>

#include "RoadState.cpp"

class Vehicle
{
    protected:
        WINDOW* win;
        char* symbol;
        RoadState road_state;
        std::pair<int,int> position;

    public:
        virtual void move() = 0;
};