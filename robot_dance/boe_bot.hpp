#ifndef boe_bot_h_
#define boe_bot_h_

#include <Arduino.h>

#include "location.h"
#include "sensors.hpp"
#include "wheel_control.hpp"
#include "permanent_storage.hpp"

#define VERSION "001"


class boe_bot {

    const uint8_t led = 11;
    const uint8_t button = 2;

    int current_instruction = 0;

    location location_state;

    wheel_control wheels;
    sensors ir_sensors;
    permanent_storage storage;

public:

    /**
     * Gets sensors states containing last measurements.
     *
     * @return The sensors states containing last measurements.
     */
    sensors get_sensors() const {
        return ir_sensors;
    }

    /**
     * Gets current location of the robot.
     *
     * @return The current location of the robot.
     */
    const location &get_location() const {
        return location_state;
    }

    /**
     * Sets new current location of the robot.
     *
     * @param loc The new current location of the robot.
     */
    void set_location(const location &loc) {
        location_state = loc;
    }

    /**
    * Turns the LED on.
    */
    void led_on() {
        digitalWrite(led, HIGH);
    }

    /**
    * Turns the LED off.
    */
    void led_off() {
        digitalWrite(led, LOW);
    }

    /**
    * Zastaví kola robota, kvůli špatné kalibraci k úplnému zastavení dojít nemusí.
    */
    void stop() {
        wheels.left_speed(0);
        wheels.right_speed(0);
    }

    /**
    * Dopředná jízda plnou rychlostí.
    */
    void full_forward() {
        wheels.left_speed(1);
        wheels.right_speed(1);
    }

    /**
    * Move forward with half speed.
    */
    void half_forward() {
        wheels.left_speed(0.5);
        wheels.right_speed(0.5);
    }

    /**
    * Move forward with quarter speed.
    */
    void quarter_forward() {
        wheels.left_speed(0.25);
        wheels.right_speed(0.25);
    }

    /**
    * Otáčení doleva na místě.
    */
    void in_place_left() {
        wheels.left_speed(-1);
        wheels.right_speed(1);
    }

    /**
    * Pomalejší zatáčení doleva obloukem.
    */
    void slightly_left() {
        wheels.left_speed(-0.05);
        wheels.right_speed(0.2);
    }

    /**
    * Nejprudší zatáčení doleva obloukem.
    */
    void sharply_left() {
        wheels.left_speed(0);
        wheels.right_speed(1);
    }

    /**
    * Otáčení doprava na místě.
    */
    void in_place_right() {
        wheels.left_speed(1);
        wheels.right_speed(-1);
    }

    /**
    * Pomalejší zatáčení doprava obloukem.
    */
    void slightly_right() {
        wheels.left_speed(0.2);
        wheels.right_speed(-0.05);
    }

    /**
    * Nejprudší zatáčení doprava obloukem.
    */
    void sharply_right() {
        wheels.left_speed(1);
        wheels.right_speed(0);
    }

    void setup() {
        /* Inicializace výstupu po sériové lince */
        Serial.begin(9600);
        Serial.println("Serial prepared");


        /* Nastavení vstupů */
        pinMode(button, INPUT);
        /* Tlačítko se zapnutým pull-up -> stisk vyvolá hodnotu LOW */
        digitalWrite(button, HIGH);
        ir_sensors.init_sensors();


        /* Nastavení výstupů */
        pinMode(led, OUTPUT);
        wheels.init_servos();
    }

    void start() {
        /* Aktivní čekání na první stisknutí tlačítka + umožnění nahrání tance */
        int in;
        boolean writingStarted = false;
        int address = 0;
        do {
            /* Jsou dostupná data na vstupu? */
            if (Serial.available() > 0) {
                int incomingByte = Serial.read();

                /* Zápis unikátní konstanty pro snadnou kontrolu konzistence */
                if (!writingStarted) {
                    storage.text_to_EEPROM(address, sizeof(MAGIC), MAGIC);
                    address += sizeof(MAGIC);
                    writingStarted = true;
                }

                /* Uložení (části) příkazu */
                EEPROM.write(address++, incomingByte);
            }

            /* Kontrola tlačítka */
            in = digitalRead(button);
        } while (in != 0);
    }

};

#endif
