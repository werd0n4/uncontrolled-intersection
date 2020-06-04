#include <ncurses.h>
#include <mutex>
#include <thread>
#include "RoadState.cpp"




class Car 
{
    private:
        int delta_y_to_do = 0;
        int delta_x_to_do = 0;
        int current_delta_y = 0;
        int current_delta_x = 0;
        WINDOW* win;
        const char* symbol;
        bool hasArrived;
        int speed;
    public:
        RoadState* road_state;
        std::pair<int,int> position, newPosition;//(y,x)
        Road_Pos start_pos;

    Car(WINDOW* _win, RoadState* _road_state, Road_Pos _start_pos, const char* _symbol) : win(_win), road_state(_road_state), 
                                                                                    symbol(_symbol), start_pos(_start_pos),
                                                                                    hasArrived(false), speed(500)
    {
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
        newPosition = position;
    }

    ~Car(){
        road_state->setPositionFree(position.second, position.first);
    }

    void set_on_junction(){
        road_state->setPositionOccupied(position.second, position.first);
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
            newPosition.second--;
            break;
        case LEFT:
            newPosition.second++;
            break;
        case TOP:
            newPosition.first++;
            break;
        case BOT:
            newPosition.first--;
        }
    }

    void step_x_right(Road_Pos start_pos){
        switch (start_pos)
        {
        case RIGHT:
            newPosition.first--;
            break;
        case LEFT:
            newPosition.first++;
            break;
        case TOP:
            newPosition.second--;
            break;
        case BOT:
            newPosition.second++;
            break;
        }
    }

    void step_x_left(Road_Pos start_pos){
        switch (start_pos)
        {
        case RIGHT:
            newPosition.first++;
            break;
        case LEFT:
            newPosition.first--;
            break;
        case TOP:
            newPosition.second++;
            break;
        case BOT:
            newPosition.second--;
            break;
        }
    }

    void calculate_next_position(Movement_direction dir){
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
    }

    void move(Movement_direction dir)
    {
        bool moveFinished = false;

        calculate_next_position(dir);
        if(hasArrived){
            road_state->setPositionFree(position.second, position.first);
            return;
        }
        while(!moveFinished){
            if((road_state->getPositionStatus(newPosition.second, newPosition.first) == false && checkIfYouCanPassCrossing(position, newPosition)) || 
                (road_state->checkIfCrossingIsBlocked() && start_pos == BOT)){//check if position is already occupied or crossing is blocked

                //position is free
                road_state->setPositionOccupied(newPosition.second, newPosition.first);
                road_state->setPositionFree(position.second, position.first);
                position = newPosition;
                moveFinished = true;
            }
            else{
                std::this_thread::sleep_for(std::chrono::milliseconds(speed));
            }
        }
    }

    //return true if there is a car on your path on crossing
    bool checkIfYouCanPassCrossing(std::pair<int, int> position, std::pair<int, int> newPosition){
        if(position == road_state->getStopLine(start_pos)){
            return !checkRightSide(position, newPosition);
        }
        else{
            return true;
        }
    }

    //return true if there is a car to the right of the newPosition
    bool checkRightSide(std::pair<int, int> position, std::pair<int, int> newPosition){
        if(position.first - newPosition.first != 0){//car is entering in vertical line
            if(position.first - newPosition.first > 0){//car is entering from bot to top
                return road_state->getPositionStatus(position.second+1, position.first-2)//default case to check
                    || road_state->getPositionStatus(position.second+1, position.first-1)
                    || road_state->getPositionStatus(position.second+1, position.first)
                    || road_state->getPositionStatus(position.second+2, position.first-2);
                    // || road_state->getPositionStatus(position.second, position.first-2);
            }
            else{//car is moving from top to bot
                return road_state->getPositionStatus(position.second-1, position.first+2)//default case to check
                    || road_state->getPositionStatus(position.second-1, position.first+1)
                    || road_state->getPositionStatus(position.second-1, position.first)
                    || road_state->getPositionStatus(position.second-2, position.first+2);
                    // || road_state->getPositionStatus(position.second, position.first+2);
            }
        }
        else{//car is entering in horizontal line 
            if(position.second - newPosition.second < 0){//car is entering from left to right
                return road_state->getPositionStatus(position.second+2, position.first+1)//default case to check
                    || road_state->getPositionStatus(position.second+1, position.first+1)
                    || road_state->getPositionStatus(position.second, position.first+1)
                    || road_state->getPositionStatus(position.second+2, position.first+2);
                    // || road_state->getPositionStatus(position.second+2, position.first);
            }
            else{//car is entering from right to left
                return road_state->getPositionStatus(position.second-2, position.first-1)//default case to check
                    || road_state->getPositionStatus(position.second-1, position.first-1)
                    || road_state->getPositionStatus(position.second, position.first-1)
                    || road_state->getPositionStatus(position.second-2, position.first-2);
                    // || road_state->getPositionStatus(position.second-2, position.first);
            }
        }
    }

    bool getHasArrived(){
        return this->hasArrived;
    }

    int getSpeed(){
        return this->speed;
    }

    bool isStartPositionOccupied(){
        return road_state->getPositionStatus(position.second, position.first);
    }

    void slowDown(){
        if(this->speed < 1000)
            this->speed += 100;
    }

    const char* getSymbol(){
        return symbol;
    }

    std::pair<int, int> getPosition(){
        return position;
    }
};
