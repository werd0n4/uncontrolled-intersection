#include <tuple>

struct RoadState {
    int slots;
    int lanes;
    int wall;
    std::tuple<int, int> top, bot, left, right;//tuple contains start positions on roads (y,x)

    RoadState(){
        this->slots=0;
        this->lanes=0;
        wall=0;
    }

    RoadState(int lanes, int slots){
        this->slots = slots;
        this->lanes = lanes;
        wall = 2*slots + lanes;
        top = std::make_tuple(1, slots + 1);
        bot = std::make_tuple(2*slots + lanes, slots+lanes);
        left = std::make_tuple(slots + lanes, 1);
        right = std::make_tuple(slots + 1, 2*slots + lanes);
    }
};