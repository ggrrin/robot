
#ifndef COMMAND_PARSER_EEPROM_HPP
#define COMMAND_PARSER_EEPROM_HPP

#include <EEPROM.h>

#include "planning.h"

#define MAGIC           ("GV-01")
#define DEFAULT_DANCE   ("A1N 2B T0 3C T0 4D T100 1A T0 A4 T200 B3 T0 C2 T0 D1 T300 1A T0")


class command_parser_eeprom : public command_parser {

    /**
     * All states of parsing state automaton.
     */
    enum parser_state {
        FIRST,
        X_FIRST,
        Y_FIRST,
        XX_FIRST,
        YY_FIRST,
        ORIENTATION_FIRST,
        NEXT,
        X_NEXT,
        Y_NEXT,
        XX_NEXT,
        YY_NEXT,
        LOC_DONE,
        TIME
    };

    parser_state current_state = FIRST;

    int current_instruction = 0;
    int current_address = 0;
    String buffer;

    int parsed_vertical = 0;
    int parsed_horizontal = 0;
    direction parsed_direction = direction::NotSpecified;

    time_type parsed_time_constrain = 0;
    bool is_x_preferred = false;
    bool is_next_fetched = false;

    position parsed_position;
    location initial_location;

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

    /**
     * Writes magic beginning on the current address.
     */
    void write_magic();

    /**
     * Checks if magic at the beginning of the EEPROM matches.
     *
     * @return If the magic at the beginning of the EEPROM matches.
     */
    bool check_magic();

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

command_parser_eeprom::command_parser_eeprom() : current_state(FIRST) {
    Serial.println(F("First few chars from EEPROM:"));
    for (int i = 0; i < 128; ++i) {
        Serial.print((char) EEPROM.read(i));
    }
    Serial.println(F("..."));

    if (!check_magic()) {
        Serial.println(F("Magic not found, writing default..."));
        reset_commands();
        return;
    }
}

bool command_parser_eeprom::fetch_initial() {
    Serial.println(F("Fetching initial position..."));

    bool _x;
    char next_character;
    current_address = sizeof(MAGIC);
    current_state = parser_state::FIRST;

    /* Parse whole first instruction from EEPROM */
    bool is_ok;
    do {
        next_character = (char) EEPROM.read(current_address++);
        is_ok = parse_next_character(next_character, &_x);
    } while (current_state != parser_state::NEXT && is_ok);
    ++current_instruction;

    if (!is_ok) {
        current_instruction = 0;
        current_address = sizeof(MAGIC);
        current_state = parser_state::FIRST;
        Serial.println(F("Dance empty or invalid! Writing default..."));
        reset_commands();
        return false;
    }
    return true;
}

bool command_parser_eeprom::fetch_next() {
	Serial.println("fetch_next()");
    bool _x;
    char next_character;
    is_next_fetched = false;

    /* Parse whole new instruction from EEPROM */
    bool is_ok;
    do {
        next_character = (char) EEPROM.read(current_address++);
	Serial.println((char)next_character);
        is_ok = parse_next_character(next_character, &_x);
    } while (current_state != parser_state::NEXT && is_ok);
    ++current_instruction;

    return is_next_fetched;
}

location command_parser_eeprom::get_initial_location() {
	parsed_time_constrain = 0;
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
    bool is_ok = parse_next_character(character, &write_to_eeprom);

    if (write_to_eeprom) {
		Serial.print("add=");
		Serial.println(current_address);
    	Serial.println(character);
        EEPROM.write(current_address++, (uint8_t) character);
        EEPROM.write(current_address + 0, ' ');
        EEPROM.write(current_address + 1, '#');
    }

    return is_ok;
}

void command_parser_eeprom::reset_commands() {
    current_address = 0;
    buffer = "";
    write_magic();

    /* Write default dance sequence */
    for (int i = 0; i < sizeof(DEFAULT_DANCE) - 1; ++i) {
        EEPROM.write(current_address++, (uint8_t) DEFAULT_DANCE[i]);
    }
    EEPROM.write(current_address + 0, ' ');
    EEPROM.write(current_address + 1, '#');

    /* Initialize the variables */
    current_address = sizeof(MAGIC);
    current_state = parser_state::FIRST;
}

bool command_parser_eeprom::check_magic() {
    for (int i = 0; i < sizeof(MAGIC) - 1; ++i) {
        if ((char) EEPROM.read(i) != MAGIC[i]) {
            return false;
        }
    }
    return true;
}

void command_parser_eeprom::write_magic() {
    for (; current_address < sizeof(MAGIC) - 1; ++current_address) {
        EEPROM.write(current_address, (byte) MAGIC[current_address]);
    }
    EEPROM.write(current_address++, ' ');
    EEPROM.write(current_address, '#');
}

bool command_parser_eeprom::parse_next_character(const char &character, bool *write_to_eeprom) {
    char buff[40];
    sprintf(buff, "char %0X = '%c', state = %d", character, character, current_state);
    Serial.println(buff);

    *write_to_eeprom = true;
    switch (current_state) {
        case FIRST:
            if (isalpha(character)) {
                parsed_horizontal = tolower(character) - 'a';
                current_state = parser_state::XX_FIRST;
            } else if (isdigit(character)) {
                parsed_vertical = character - '0';
                current_state = parser_state::YY_FIRST;
            } else if (isblank(character) || character == '\n') {
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
                initial_location.set_location(parsed_horizontal, parsed_vertical - 1, parsed_direction);
                sprintf(buff, "parsed initial dir=%d, loc=(%d,%d)", initial_location.get_direction(),
                        initial_location.get_position().get_x(), initial_location.get_position().get_y());
                Serial.println(buff);
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
                initial_location.set_location(parsed_horizontal, parsed_vertical - 1, parsed_direction);
                sprintf(buff, "parsed initial dir=%d, loc=(%d,%d)", initial_location.get_direction(),
                        initial_location.get_position().get_x(), initial_location.get_position().get_y());
                Serial.println(buff);
                current_state = parser_state::ORIENTATION_FIRST;
            } else {
                return false;
            }
            break;
        case ORIENTATION_FIRST:
            if (isblank(character) || character == '\n') {
                current_state = parser_state::NEXT;
            } else {
                return false;
            }
            break;
        case NEXT:
            if (isalpha(character)) {
                is_x_preferred = true;
                parsed_time_constrain = 0;
                parsed_horizontal = tolower(character) - 'a';
                current_state = parser_state::XX_NEXT;
            } else if (isdigit(character)) {
                is_x_preferred = false;
                parsed_time_constrain = 0;
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
                current_state = parser_state::TIME;
            } else if (isblank(character)) {
                current_state = parser_state::LOC_DONE;
            } else {
                return false;
            }
            parsed_position = position(parsed_horizontal, parsed_vertical - 1);
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
                current_state = parser_state::TIME;
            } else if (isblank(character)) {
                current_state = parser_state::LOC_DONE;
            } else {
                return false;
            }
            parsed_position = position(parsed_horizontal, parsed_vertical - 1);
            break;
        case LOC_DONE:
            if (toupper(character) == 'T') {
                current_state = parser_state::TIME;
            } else if (isblank(character)) {
                *write_to_eeprom = false;
            }
            break;
        case TIME:
            if (isdigit(character)) {
                parsed_time_constrain = parsed_time_constrain * 10 + (character - '0');
            } else if (isblank(character) || character == '\n') {
                sprintf(buff, "parsed pos=(%d,%d), time=%lu, x_preferred=%d", parsed_position.get_x(),
                        parsed_position.get_y(), parsed_time_constrain, is_x_preferred);
                Serial.println(buff);
                is_next_fetched = true;
                current_state = parser_state::NEXT;
            } else {
                return false;
            }
            break;
    }

    return true;
}

#endif //COMMAND_PARSER_EEPROM_HPP
