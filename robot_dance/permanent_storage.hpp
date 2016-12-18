
#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <Arduino.h>
#include <EEPROM.h>

#define MAGIC   ("GV-v" VERSION)


/**
 * Class providing access to read and write from/to permanent EEPROM storage.
 */
class permanent_storage {

public:

    /**
     * Saves given text to EEPROM at given adress with given length. The remaining space
     * will be filled with empty spaces and written as well.
     *
     * @param address Address in EEPROM, which should contain the first byte.
     * @param length Total number of bytes to be used.
     * @param text String to be stored within the EEPROM.
     */
    void text_to_EEPROM(int address, int length, String text);

    /**
     * Reads text of a given length from the EEPROM.
     *
     * @param address Address of the first byte in EEPROM.
     * @param length Length of the text stored in EEPROM.
     * @return Read text without the trailing spaces.
     */
    String text_from_EEPROM(int address, int length);

    /**
     * Writes an integer value to EEPROM.
     *
     * @param address Address in EEPROM of the first byte.
     * @param value Integer value to be stored.
     */
    void int_to_EEPROM(int address, int value);

    /**
     * Reads an integer value from EEPROM.
     *
     * @param address Address of the first byte in EEPROM.
     * @return Read integer value from the EEPROM.
     */
    int int_from_EEPROM(int address);

};



//class permanent_storage

void permanent_storage::text_to_EEPROM(int address, int length, String text) {
    for (int i = 0; i < text.length(); i++) {
        EEPROM.write(address + i, (byte) text[i]);
    }
    for (int i = text.length(); i < length; i++) {
        EEPROM.write(address + i, ' ');
    }
}

String permanent_storage::text_from_EEPROM(int address, int length) {
    String text = "";
    for (int i = 0; i < length; i++) {
        char character = (char) EEPROM.read(address + i);
        text = text + character;
    }
    while (text[text.length() - 1] == ' ') {
        text = text.substring(0, text.length() - 1);
    }
    return text;
}

void permanent_storage::int_to_EEPROM(int address, int value) {
    byte byte1 = (byte) value & 0xFF;
    byte byte2 = (byte) ((value >> 8) & 0xFF);
    EEPROM.write(address, byte1);
    EEPROM.write(address + 1, byte2);
}

int permanent_storage::int_from_EEPROM(int address) {
    return EEPROM.read(address) + ((EEPROM.read(address + 1)) << 8);
}

#endif //STORAGE_HPP
