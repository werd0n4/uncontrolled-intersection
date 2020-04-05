#include <tuple>
#include <vector>

struct RoadState {
    int slots;
    int lanes;
    int wall;
    std::tuple<int, int> start_positions[4];// top, right, bot, left;//tuple contains start start_positions on roads (y,x)
    std::tuple<int, int> end_positions[4];//top, right, bot, left
    std::vector<std::vector<bool>> OCCUPIED_POSITIONS;

    RoadState(){
        this->slots=0;
        this->lanes=0;
        wall=0;
    }

    RoadState(int lanes, int slots){        
        this->slots = slots;
        this->lanes = lanes;
        wall = 2*slots + lanes;
        start_positions[0] = std::make_tuple(1, slots + 1);//top
        end_positions[0] = std::make_tuple(1, slots + 2);

        start_positions[2] = std::make_tuple(2*slots + lanes, slots+lanes);//bot
        end_positions[2] = std::make_tuple(2*slots + lanes, slots + lanes - 1);
        
        start_positions[3] = std::make_tuple(slots + lanes, 1);//left
        end_positions[3] = std::make_tuple(slots + lanes - 1, 1);

        start_positions[1] = std::make_tuple(slots + 1, 2*slots + lanes);//right
        end_positions[1] = std::make_tuple(slots + 2, 2*slots + lanes);

        OCCUPIED_POSITIONS.resize(wall+2);
        for (int i = 0; i < wall+2; i++)
        {
            OCCUPIED_POSITIONS[i].resize(wall+2);
        }

        // for(int i=0; i < wall+2; ++i){
        //     for (int j=0; j< wall+2;++j){
        //         OCCUPIED_POSITIONS[i][j] = false;
        //     }
        // }
    }
};