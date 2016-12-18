
#ifndef WHEEL_CONTROL_H
#define WHEEL_CONTROL_H

#include <Arduino.h>
#include <Servo.h>

#define MIN_LEFT_SPEED  (1300)
#define MAX_LEFT_SPEED  (1700)
#define MIN_RIGHT_SPEED (1700)
#define MAX_RIGHT_SPEED (1300)

/**
 * Serva pro ovládání kol.
 */
Servo leftWheel;
Servo rightWheel;


/**
 * Inicializace serv kol na daných pinech.
 *
 * @param leftWheelPin Pin levého serva.
 * @param rightWheelPin Pin pravého serva.
 */
void servo_init(const uint8_t leftWheelPin, const uint8_t rightWheelPin) {
    leftWheel.attach(leftWheelPin);
    rightWheel.attach(rightWheelPin);
}

/**
 * Přeškáluje hodnotu z [-1; 1] a spustí levé servo odpovídajícím směrem a rychlostí.
 * -1 je maximum vzad, 0 stop, 1 maximum vpřed.
 *
 * @param speed Rychlost levého kola z intervalu [-1; 1], speciálně 0 znamená zastavení.
 */
void left_speed(double speed) {
    leftWheel.writeMicroseconds((int) ((MAX_LEFT_SPEED - MIN_LEFT_SPEED) * (speed + 1) / 2.0 + MIN_LEFT_SPEED));
}

/**
 * Přeškáluje hodnotu z [-1; 1] a spustí pravé servo odpovídajícím směrem a rychlostí.
 * -1 je maximum vzad, 0 stop, 1 maximum vpřed.
 *
 * @param speed Rychlost pravého kola z intervalu [-1; 1], speciálně 0 znamená zastavení.
 */
void right_speed(double speed) {
    rightWheel.writeMicroseconds((int) ((MAX_RIGHT_SPEED - MIN_RIGHT_SPEED) * (speed + 1) / 2.0 + MIN_RIGHT_SPEED));
}

/**
 * Zastaví kola robota, kvůli špatné kalibraci k úplnému zastavení dojít nemusí.
 */
void stop() {
    left_speed(0);
    right_speed(0);
}

/**
 * Dopředná jízda plnou rychlostí.
 */
void full_forward() {
    left_speed(1);
    right_speed(1);
}

/**
 * Dopředná jízda poloviční rychlostí z maxima.
 */
void half_forward() {
    left_speed(0.5);
    right_speed(0.5);
}

/**
 * Otáčení doleva na místě.
 */
void in_place_left() {
    left_speed(-1);
    right_speed(1);
}

/**
 * Pomalejší zatáčení doleva obloukem.
 */
void slightly_left() {
    left_speed(-0.05);
    right_speed(0.2);
}

/**
 * Nejprudší zatáčení doleva obloukem.
 */
void sharply_left() {
    left_speed(0);
    right_speed(1);
}

/**
 * Otáčení doprava na místě.
 */
void in_place_right() {
    left_speed(1);
    right_speed(-1);
}

/**
 * Pomalejší zatáčení doprava obloukem.
 */
void slightly_right() {
    left_speed(0.2);
    right_speed(-0.05);
}

/**
 * Nejprudší zatáčení doprava obloukem.
 */
void sharply_right() {
    left_speed(1);
    right_speed(0);
}

#endif //WHEEL_CONTROL_H
