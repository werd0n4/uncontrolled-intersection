#include <ncurses.h>
#include <mutex>
#include <thread>
#include "Vehicle.cpp"

class Emergency : public Vehicle
{
    private:
        int delta_y_to_do = 0;
        int delta_x_to_do = 0;
        int current_delta_y = 0;
        int current_delta_x = 0;
        std::mutex mtx;

    public:
    Emergency(WINDOW* win, RoadState* road_state, Road_Pos start)
    {
        this->road_state = road_state;
        this->win = win;
        this->symbol = (char*)"E";
        this->start_pos = start;
        // this->set_on_junction(start_pos); 
        this->hasArrived = false;
        this->default_speed = 500;

        switch(this->start_pos){
            case 0://top
                position.first = 1;
                position.second = road_state->slots+1;
                break;
            case 2://bot
                position.first = 2*road_state->slots + road_state->lanes;
                position.second = road_state->slots+road_state->lanes;
                break;
            case 3://left
                position.first = road_state->slots + road_state->lanes;
                position.second = 1;
                break;
            case 1://right
                position.first = road_state->slots+1;
                position.second = 2*road_state->slots + road_state->lanes;
                break;
        }
    }

    ~Emergency(){
        mvwprintw(win, position.first, position.second, ".");
        wrefresh(win);
    }

    void set_on_junction()
    {
        road_state->OCCUPIED_POSITIONS[position.second][position.first] = true;
        mvwprintw(win, position.first, position.second, symbol);
        wrefresh(win);
    }

    void calculate_movement_to_do(Movement_direction dir){
        switch(dir){
            case FORWARD:
                delta_y_to_do = 2*road_state->slots + road_state->lanes-1;
                delta_x_to_do = 0;
                break;
            case TURN_RIGHT:
                delta_y_to_do = road_state->slots;
                delta_x_to_do = road_state->slots;
                break;
            case TURN_LEFT:
                delta_y_to_do = road_state->slots + road_state->lanes-1;
                delta_x_to_do = road_state->slots + road_state->lanes-1;
                break;
        }
    }

    void step_y(Road_Pos start_pos){

        switch (start_pos)
        {
        case RIGHT:
            position.second--;
            break;
        case LEFT:
            position.second++;
            break;
        case TOP:
            position.first++;
            break;
        case BOT:
            position.first--;
        }
    }

    void step_x_right(Road_Pos start_pos){
        switch (start_pos)
        {
        case RIGHT:
            position.first--;
            break;
        case LEFT:
            position.first++;
            break;
        case TOP:
            position.second--;
            break;
        case BOT:
            position.second++;
            break;
        }
    }

    void step_x_left(Road_Pos start_pos){
        switch (start_pos)
        {
        case RIGHT:
            position.first++;
            break;
        case LEFT:
            position.first--;
            break;
        case TOP:
            position.second++;
            break;
        case BOT:
            position.second--;
            break;
        }
    }

    void move(Movement_direction dir)
    {

        mvwprintw(win, position.first, position.second, "."); // zwolnienie pozycji
        road_state->OCCUPIED_POSITIONS[position.second][position.first] = false;

        if(current_delta_y < delta_y_to_do){
            step_y(start_pos);
            ++current_delta_y;
        }
        else if(current_delta_x < delta_x_to_do){
            if(dir == TURN_LEFT)
                step_x_left(start_pos);
            if(dir == TURN_RIGHT)
                step_x_right(start_pos);

            ++current_delta_x;
        }
        else{
            this->hasArrived = true;
        }
        road_state->OCCUPIED_POSITIONS[position.second][position.first] = true;
        mvwprintw(win, position.first, position.second, symbol);
        wrefresh(win);
    }

    bool getHasArrived(){
        return this->hasArrived;
    }

    int getDefaultSpeed(){
        return this->default_speed;
    }

    bool isStartPositionOccupied(){
        return road_state->OCCUPIED_POSITIONS[position.second][position.first];
    }
};
