#include <ncurses.h>

#include "RoadState.cpp"

enum Road_Pos{
    TOP, RIGHT, BOT, LEFT
};

enum Movement_direction{
    FORWARD, TURN_RIGHT, TURN_LEFT
};

class Vehicle
{
    protected:
        WINDOW* win;
        char* symbol;
        RoadState road_state;
        std::pair<int,int> position;//(y,x)
        Road_Pos start_pos;
        
    public:
        virtual void move(Movement_direction) = 0;
};