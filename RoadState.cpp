#include <tuple>
#include <vector>
#include <atomic>
#include <iostream>
#include <mutex>

class RoadState {
    public:
    int slots;
    int lanes;
    int wall;
    std::mutex mtx1;
    std::mutex mtx2;
    std::mutex mtx3;
    std::tuple<int, int> start_positions[4];// top, right, bot, left;//tuple contains start start_positions on roads (y,x)
    std::tuple<int, int> end_positions[4];//top, right, bot, left
    std::vector<std::vector<bool>> OCCUPIED_POSITIONS;


    RoadState(): slots(0), lanes(0), wall(0){ }

    RoadState(int _lanes, int _slots) : lanes(_lanes), slots(_slots), wall(2*slots+lanes)
    {   
        start_positions[0] = std::make_tuple(1, slots + 1);//top
        end_positions[0] = std::make_tuple(1, slots + 2);

        start_positions[2] = std::make_tuple(2*slots + lanes, slots+lanes);//bot
        end_positions[2] = std::make_tuple(2*slots + lanes, slots + lanes - 1);
        
        start_positions[3] = std::make_tuple(slots + lanes, 1);//left
        end_positions[3] = std::make_tuple(slots + lanes - 1, 1);

        start_positions[1] = std::make_tuple(slots + 1, 2*slots + lanes);//right
        end_positions[1] = std::make_tuple(slots + 2, 2*slots + lanes);

        for(int i=0;i<wall+2;++i){
            std::vector<bool> temp;
            for(int j=0;j<wall+2;++j){
                temp.push_back(false);
            }
            OCCUPIED_POSITIONS.push_back(temp);
        }
    }

    void setPositionOccupied(int y, int x)
    {
        mtx1.lock();
        OCCUPIED_POSITIONS[y][x] = true;
        mtx1.unlock();
    }

    void setPositionFree(int y, int x)
    {
        mtx2.lock();
        OCCUPIED_POSITIONS[y][x] = false;
        mtx2.unlock();
    }

    bool getPositionStatus(int y, int x)
    {
        bool status;
        mtx3.lock();
        status = OCCUPIED_POSITIONS[y][x];
        mtx3.unlock();
        return status;
    }
};