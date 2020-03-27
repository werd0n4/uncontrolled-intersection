#include <ncurses.h>

#include "Vehicle.cpp"

class Emergency : protected Vehicle
{
    private:
        int i = 1;

    public:
    Emergency(WINDOW* win, RoadState road_state, int start)
    {
        this->road_state = road_state;
        set_on_junction(start); 
        this->win = win;
        this->symbol = (char*)"E";
    }

    void set_on_junction(int start)
    {
        switch(start){
            case 0://top
                position.first = 1;
                position.second = road_state.slots+1;
                break;
            case 2://bot
                position.first = 2*road_state.slots + road_state.lanes;
                position.second = road_state.slots+road_state.lanes;
                break;
            case 3://left
                position.first = road_state.slots + road_state.lanes;
                position.second = 1;
                break;
            case 1://right
                position.first = road_state.slots+1;
                position.second = 2*road_state.slots + road_state.lanes;
                break;
        }
        mvwprintw(win, position.first, position.second, "E");
    }

    void move()
    {
        mvwprintw(win, position.first, position.second, ".");
        if(position.first >road_state.lanes + road_state.slots){
            --position.first;
        }
        else if(position.first < 2*road_state.slots + road_state.lanes){
            ++position.second;
        }
        mvwprintw(win, position.first, position.second, "E");
    }
};
