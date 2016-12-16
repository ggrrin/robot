
#ifndef ROBOT_FOLLOWER_HINTS_BUTTON_H
#define ROBOT_FOLLOWER_HINTS_BUTTON_H

#include <Arduino.h>

#define DEBOUNCE_TIME   (50)
#define LONG_PRESS_TIME (500)


/**
 * Inicializace tlačítkového pinu.
 *
 * @param button Pin s připojeným tlačítkem.
 */
void init_button(const uint8_t button) {
    pinMode(button, INPUT);
    /* Tlačítko se zapnutým pull-up -> stisk vyvolá hodnotu LOW */
    digitalWrite(button, HIGH);

}

/**
 * Aktivně počká, až na tlačítku bude daná hodnota. Využívá debounce pro
 * odstranění šumu.
 *
 * @param button Pin, kde je připojeno tlačítko.
 * @param desiredValue Hodnota, na kterou se na tlačítku čeká.
 */
void wait_for_button(const uint8_t button, const int desiredValue) {
    int in1, in2;
    do {
        in1 = digitalRead(button);
        delay(DEBOUNCE_TIME);
        in2 = digitalRead(button);
    } while (in1 != desiredValue || in1 != in2);
}

/**
 * Aktivně počká na stisk tlačítka a zjistí, jestli šlo o běžný, nebo dlouhý stisk.
 *
 * @param button Pin, kde je připojeno tlačítko.
 * @return Jestli šlo o dlouhý stisk.
 */
boolean check_long_press_button(const uint8_t button) {
    /* Vyčkání na stisk tlačítka */
    wait_for_button(button, LOW);
    unsigned long pressStart = millis();

    /* Čekání na puštění tlačítka */
    wait_for_button(button, HIGH);

    return (millis() - pressStart) > LONG_PRESS_TIME;
}

#endif //ROBOT_FOLLOWER_HINTS_BUTTON_H
