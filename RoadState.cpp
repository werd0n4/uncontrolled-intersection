#include <tuple>
#include <vector>
#include <atomic>
#include <iostream>
#include <mutex>

enum Road_Pos{
    TOP, RIGHT, BOT, LEFT
};

enum Movement_direction{
    FORWARD, TURN_RIGHT, TURN_LEFT
};

class RoadState {
    public:
    int slots;
    int lanes;
    int wall;
    std::mutex mtx;
    std::tuple<int, int> start_positions[4];// top, right, bot, left;//tuple contains start start_positions on roads (y,x)
    std::tuple<int, int> end_positions[4];//top, right, bot, left
    std::pair<int, int> stop_line_position[4];//top, right, bot, left
    std::vector<std::vector<bool>> OCCUPIED_POSITIONS;


    RoadState(): slots(0), lanes(0), wall(0){ }

    RoadState(int _lanes, int _slots) : lanes(_lanes), slots(_slots), wall(2*slots+lanes)
    {   
        start_positions[0] = std::make_tuple(1, slots + 1);//top
        end_positions[0] = std::make_tuple(1, slots + 2);
        stop_line_position[0] = std::make_pair(slots, slots+1);

        start_positions[2] = std::make_tuple(2*slots + lanes, slots+lanes);//bot
        end_positions[2] = std::make_tuple(2*slots + lanes, slots + lanes - 1);
        stop_line_position[2] = std::make_pair(slots+lanes+1, slots+2);
        
        start_positions[3] = std::make_tuple(slots + lanes, 1);//left
        end_positions[3] = std::make_tuple(slots + lanes - 1, 1);
        stop_line_position[3] = std::make_pair(slots+lanes, slots);

        start_positions[1] = std::make_tuple(slots + 1, 2*slots + lanes);//right
        end_positions[1] = std::make_tuple(slots + 2, 2*slots + lanes);
        stop_line_position[1] = std::make_pair(slots+1, slots+lanes+1);

        for(int i=0;i<wall+2;++i){
            std::vector<bool> temp;
            for(int j=0;j<wall+2;++j){
                temp.push_back(false);
            }
            OCCUPIED_POSITIONS.push_back(temp);
        }
    }

    std::pair<int, int> getStopLine(Road_Pos dir){
        return stop_line_position[dir];
    }

    void setPositionOccupied(int y, int x)
    {
        std::lock_guard<std::mutex> lg(mtx);
        OCCUPIED_POSITIONS[y][x] = true;
    }

    void setPositionFree(int y, int x)
    {
        std::lock_guard<std::mutex> lg(mtx);
        OCCUPIED_POSITIONS[y][x] = false;
    }

    // return true if position is occupied
    bool getPositionStatus(int y, int x)
    {
        bool status;
        std::lock_guard<std::mutex> lg(mtx);
        status = OCCUPIED_POSITIONS[y][x];
        return status;
    }

    // return if every stop line position is occupied, if so then first car that notice it should drive
    bool checkIfCrossingIsBlocked()
    {
        bool result = getPositionStatus(stop_line_position[0].second, stop_line_position[0].first) &&
                    getPositionStatus(stop_line_position[1].second, stop_line_position[1].first) &&
                    getPositionStatus(stop_line_position[2].second, stop_line_position[2].first) &&
                    getPositionStatus(stop_line_position[3].second, stop_line_position[3].first);
        return result;
    }
};