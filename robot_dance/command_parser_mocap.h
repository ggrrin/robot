#ifndef command_parser_mocap_h
#define command_parser_mocap_h

#include "planning.h"


//#define MOVE
//#define TURNL
//#define TURNR
//#define CMP
//#define CMP2
#define CMP3


class command_parser_mocap : public command_parser {

    virtual bool fetch_initial() override {
        return false;
    };

    virtual bool store_character(const char &character) {
        return false;
    };

    virtual void reset_commands() {};

#ifdef MOVE
#   define size 1

    position pos[size]{
            position(2, 0),
    };

    bool firstX[size]{
            true,
    };

    time_type times[size]{
            0,
    };

    location initial_location = location(0, 0, direction::East);


#endif

#ifdef  TURNL
#   define size 1

    position pos[size]{
            position(4, 3),
    };

    bool firstX[size]{
            false,
    };

    time_type times[size]{
            0,
    };

    location initial_location = location(3, 3, direction::South);
#endif

#ifdef TURNR
#   define size 1

    position pos[size]{
            position(2, 3),
    };

    bool firstX[size]{
            true,
    };

    time_type times[size]{
            0,
    };

    location initial_location = location(3, 3, direction::South);
#endif

#ifdef CMP
#   define size 5

    position pos[size]{
//            position(2, 3),
//            position(1, 1),
//            position(0, 0),
//            position(3, 3),

            position(2, 3),
            position(2, 1),
            position(1, 2),
            position(2, 2),
            position(1, 1),

//            position(3, 0),
//            position(1, 0),
//            position(3, 0),
//            position(0, 0),
    };

    bool firstX[size]{
            false,
            true,
            false,
            false,
            false,
    };

    time_type times[size]{
            0,
            0,
            0,
            0,
            0,
    };

    location initial_location = location(0, 0, direction::West);
#endif

#ifdef CMP2
#   define size 22

    position pos[size]{
//            position(0,3),
//            position(3,3),
//            position(1,3),
//            position(2,3),
//            position(1,3),
//            position(3,3),
//            position(3,0),
//            position(0,0),
            position(3, 0),
            position(3, 3),
            position(0, 3),
            position(0, 1),
            position(2, 0),
            position(3, 2),
            position(1, 3),
            position(0, 2),
            position(1, 0),
            position(3, 1),
            position(2, 3),
            position(2, 1),
            position(1, 2),
            position(2, 2),
            position(1, 1),
            position(0, 0),
            position(3, 0),
            position(0, 0),
            position(2, 3),
            position(1, 3),
            position(2, 3),
            position(0, 0),
    };

    bool firstX[size]{
            false,
            true,
            true,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
            false,
    };

    time_type times[size]{
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
            0,
    };

    location initial_location = location(0, 0, direction::West);
#endif

#ifdef CMP3
#   define size 4

    position pos[size]{
            position(1, 1),
            position(2, 2),
            position(3, 3),
            position(0, 0),
    };

    bool firstX[size]{
            false,
            false,
            false,
            false,
    };

    time_type times[size]{
            100,
            150,
            200,
            0,
    };

    location initial_location = location(0, 0, direction::North);
#endif

    int current_instruction;

public:

    explicit command_parser_mocap();

    virtual bool fetch_next() override;

    virtual location get_initial_location() override;

    virtual position get_current_target() override;

    virtual bool is_first_directionX() override;

    virtual time_type get_finish_time_constrain() override;

};



//class command_parser_mocap

inline command_parser_mocap::command_parser_mocap() : current_instruction(-1) {}

inline bool command_parser_mocap::fetch_next() {
    current_instruction++;
    bool res = current_instruction < size;
    return res;
}

inline location command_parser_mocap::get_initial_location() {
    return initial_location;
}

inline position command_parser_mocap::get_current_target() {
    return pos[current_instruction];
}

inline bool command_parser_mocap::is_first_directionX() {
    return firstX[current_instruction];
}

inline time_type command_parser_mocap::get_finish_time_constrain() {
    return times[current_instruction];
}

#endif
