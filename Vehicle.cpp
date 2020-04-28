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
        bool hasArrived;
        int speed;
        
    public:
        RoadState road_state;
        virtual void move() = 0;
        std::pair<int,int> position;//(y,x)
        Road_Pos start_pos;
};