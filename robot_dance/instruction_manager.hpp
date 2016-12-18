
#ifndef INSTRUCTION_MANAGER_HPP
#define INSTRUCTION_MANAGER_HPP

#include <EEPROM.h>

#include "instruction.hpp"
#include "permanent_storage.hpp"

#define VERSION "001"
#define MAGIC   ("GV-v" VERSION)


class instruction_manager {

    int current_instruction = 0;
    int current_address = 0;
    String buffer;

    permanent_storage storage;

public:

    /**
     * Add one byte of data to the end of data block.
     *
     * @param value Data byte to be stored.
     */
    void store_byte(const uint8_t &value);

    //TODO
    void store_command(const instruction &instruction);

    //TODO
    const instruction &initial_instruction();

    //TODO
    const instruction &next_instruction();

    //TODO
    const bool magic_matches() const;

};



//class instruction_manager

void instruction_manager::store_command(const instruction &instruction) {
    if (current_instruction == 0) {

    }
}

const instruction &instruction_manager::initial_instruction() {
    return <#initializer#>;
}

const instruction &instruction_manager::next_instruction() {
    return <#initializer#>;
}

const bool instruction_manager::magic_matches() const {
    String text = storage.text_from_EEPROM(0, sizeof(MAGIC));
    return (const bool) text.compareTo(MAGIC);
}

void instruction_manager::store_byte(const uint8_t &value) {
    /* Write magic for simple error detection */
    if (current_address == 0) {
        storage.text_to_EEPROM(0, sizeof(MAGIC), MAGIC);
        current_address += sizeof(MAGIC);
        buffer = "";
    }

    //TODO: do bufferu a parsovat
    buffer += value;
    EEPROM.write(current_address++, value);
    EEPROM.write(current_address, '\0');
}

#endif //INSTRUCTION_MANAGER_HPP
