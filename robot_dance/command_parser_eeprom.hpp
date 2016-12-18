
#ifndef COMMAND_PARSER_EEPROM_HPP
#define COMMAND_PARSER_EEPROM_HPP

#include <EEPROM.h>

#include "permanent_storage.hpp"
#include "planning.h"

#define VERSION     "001"
#define MAGIC       ("GV-v" VERSION)
#define SEPARATOR   "\n"


class command_parser_eeprom : command_parser {

    /**
     * All states of parsing state automaton.
     */
    enum parser_state {
        FIRST, X_FIRST, Y_FIRST, XX_FIRST, YY_FIRST, ORIENTATION_FIRST, NEXT, X_NEXT, Y_NEXT, XX_NEXT, YY_NEXT, TIME
    };

    parser_state current_state;

    int current_instruction = 0;
    int current_address = 0;
    String buffer;

    int parsed_vertical = 0;
    int parsed_horizontal = 0;
    direction parsed_direction = direction::NotSpecified;

    time_type parsed_time_constrain = 0;
    bool is_x_preferred = false;

    position parsed_position;
    location initial_location;

    permanent_storage storage;

public:

    explicit command_parser_eeprom();

    /**
     * Only checks if the EEPROM has not been tinkered with.
     *
     * @return True if some sequence may be present.
     */
    virtual bool fetch_initial() override;

    virtual bool fetch_next() override;

    virtual location get_initial_location() override;

    virtual position get_current_target() override;

    virtual bool is_first_directionX() override;

    virtual time_type get_finish_time_constrain() override;

    virtual bool store_character(const char &character) override;

    virtual void reset_commands() override;

    void write_magic();

    direction parse_direction(const int &character) {
        if (character == 'N') {
            return direction::North;
        }
        if (character == 'E') {
            return direction::East;
        }
        if (character == 'S') {
            return direction::South;
        }
        if (character == 'W') {
            return direction::West;
        }
        return direction::NotSpecified;
    }

    bool parse_next_character(const char &character, bool *write_to_eeprom);

};



//class command_parser_eeprom

command_parser_eeprom::command_parser_eeprom() {
    bool _x;
    char next_character;
    current_address = sizeof(MAGIC);
    current_state = parser_state::FIRST;

    /* Parse whole first instruction from EEPROM */
    do {
        next_character = (char) EEPROM.read(current_address++);
        parse_next_character(next_character, &_x);
    } while (current_state != parser_state::NEXT);
    ++current_instruction;
}

bool command_parser_eeprom::fetch_initial() {
    String text = storage.text_from_EEPROM(0, sizeof(MAGIC));
    return (const bool) text.compareTo(MAGIC);
}

bool command_parser_eeprom::fetch_next() {
    bool _x;
    char next_character;

    /* Parse whole new instruction from EEPROM */
    do {
        next_character = (char) EEPROM.read(current_address++);
        parse_next_character(next_character, &_x);
    } while (current_state != parser_state::NEXT);
    ++current_instruction;

    return false;
}

location command_parser_eeprom::get_initial_location() {
    return initial_location;
}

position command_parser_eeprom::get_current_target() {
    return parsed_position;
}

bool command_parser_eeprom::is_first_directionX() {
    return is_x_preferred;
}

time_type command_parser_eeprom::get_finish_time_constrain() {
    return parsed_time_constrain;
}

bool command_parser_eeprom::store_character(const char &character) {
    /* Write magic for simple error detection */
    if (current_address == 0) {
        write_magic();
        buffer = "";
    }

    bool write_to_eeprom;
    parse_next_character(character, &write_to_eeprom);

    if (write_to_eeprom) {
        EEPROM.write(current_address++, (uint8_t) character);
        EEPROM.write(current_address, '\n');
    }

    return true;
}

void command_parser_eeprom::reset_commands() {
    current_address = 0;
    buffer = "";
    write_magic();
    //TODO uložit nějaký defaultní tanec
}

void command_parser_eeprom::write_magic() {
    storage.text_to_EEPROM(0, sizeof(MAGIC), MAGIC);
    storage.text_to_EEPROM(0, sizeof(SEPARATOR), SEPARATOR);
    current_address += sizeof(MAGIC) + sizeof(SEPARATOR);
}

bool command_parser_eeprom::parse_next_character(const char &character, bool *write_to_eeprom) {
    switch (current_state) {
        case FIRST:
            if (isalpha(character)) {
                parsed_horizontal = tolower(character) - 'a';
                current_state = parser_state::XX_FIRST;
            } else if (isdigit(character)) {
                parsed_vertical = character - '0';
                current_state = parser_state::YY_FIRST;
            } else if (isblank(character)) {
                *write_to_eeprom = false;
            } else {
                return false;
            }
            break;
        case XX_FIRST:
            if (isdigit(character)) {
                parsed_vertical = character - '0';
                current_state = parser_state::Y_FIRST;
            } else {
                return false;
            }
            break;
        case Y_FIRST:
            if (isdigit(character)) {
                parsed_vertical = parsed_vertical * 10 + (character - '0');
            } else if (isalpha(character)) {
                parsed_direction = parse_direction(toupper(character));
                initial_location.set_location(parsed_horizontal, parsed_vertical, parsed_direction);
                current_state = parser_state::ORIENTATION_FIRST;
            } else {
                return false;
            }
            break;
        case YY_FIRST:
            if (isdigit(character)) {
                parsed_vertical = parsed_vertical * 10 + (character - '0');
            } else if (isalpha(character)) {
                parsed_horizontal = tolower(character) - 'a';
                current_state = parser_state::X_FIRST;
            } else {
                return false;
            }
            break;
        case X_FIRST:
            if (isalpha(character)) {
                parsed_direction = parse_direction(toupper(character));
                initial_location.set_location(parsed_horizontal, parsed_vertical, parsed_direction);
                current_state = parser_state::ORIENTATION_FIRST;
            } else {
                return false;
            }
            break;
        case ORIENTATION_FIRST:
            if (character == '\n') {
                current_state = parser_state::NEXT;
            } else if (isblank(character)) {
                *write_to_eeprom = false;
            } else {
                return false;
            }
            break;
        case NEXT:
            if (isalpha(character)) {
                parsed_horizontal = tolower(character) - 'a';
                current_state = parser_state::XX_NEXT;
            } else if (isdigit(character)) {
                parsed_vertical = character - '0';
                current_state = parser_state::YY_NEXT;
            } else if (isblank(character)) {
                *write_to_eeprom = false;
            } else {
                return false;
            }
            break;
        case XX_NEXT:
            if (isdigit(character)) {
                parsed_vertical = character - '0';
                current_state = parser_state::Y_NEXT;
            } else {
                return false;
            }
            break;
        case Y_NEXT:
            if (isdigit(character)) {
                parsed_vertical = parsed_vertical * 10 + (character - '0');
            } else if (toupper(character) == 'T') {
                parsed_position = position(parsed_vertical, parsed_horizontal);
                is_x_preferred = true;
                parsed_time_constrain = 0;
                current_state = parser_state::TIME;
            } else if (isblank(character)) {
                *write_to_eeprom = false;
            } else {
                return false;
            }
            break;
        case YY_NEXT:
            if (isdigit(character)) {
                parsed_vertical = parsed_vertical * 10 + (character - '0');
            } else if (isalpha(character)) {
                parsed_horizontal = tolower(character) - 'a';
                current_state = parser_state::X_NEXT;
            } else {
                return false;
            }
            break;
        case X_NEXT:
            if (toupper(character) == 'T') {
                parsed_position = position(parsed_vertical, parsed_horizontal);
                is_x_preferred = false;
                parsed_time_constrain = 0;
                current_state = parser_state::TIME;
            } else if (isblank(character)) {
                *write_to_eeprom = false;
            } else {
                return false;
            }
            break;
        case TIME:
            if (isdigit(character)) {
                parsed_time_constrain = parsed_time_constrain * 10 + (character - '0');
            } else if (character == '\n') {
                current_state = parser_state::NEXT;
            } else if (isblank(character)) {
                *write_to_eeprom = false;
            } else {
                return false;
            }
            break;
    }

    return true;
}

#endif //COMMAND_PARSER_EEPROM_HPP
