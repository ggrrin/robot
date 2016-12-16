/**
 * Autoři: Geiger & Vašek
 * Projekt: Sledovač čáry s nápovědou u křižovatek
 * Popis:
 *      1) robot je spuštěn druhým stisknutím tlačítka
 *          - první stisknutí nastavuje, jak bude robot nakládat s nápovědou,
 *            na krátký stisk robot putuje po trase, na jejíž stranu ukazuje nápověda,
 *            na dlouhý stisk tlačítka robot vždy cestuje po opačné trase
 *      2) pokud robot nevidí čáru po stanovenou dobu (0.5s), začne
 *          spirálovité prohledávání
 *      3) robot se snaží po rovné čáře jet poloviční rychlostí
 *          - aby lépe zvládal detekci zatáček
 *      4) krajní senzory používá pouze pro detekci nápovědy
 *          - stav podle nápovědy se nemění po pevně danou dobu
 *            od její detekce pro zamezení falešných detekcí před spojením tratí
 *      5) při aktivaci mezisensorů přikládá prioritu senzoru,
 *          který odpovídá poslední zaznamenané nápovědě
 */

#include <Arduino.h>
#include <Servo.h>
#include "hints_follower.h"
#include "wheel_control.h"
#include "sensors.h"
#include "button.h"

#define SPIRAL_DELAY    (500)
#define STATE_FIXATION  (5000)


/**
 * Piny pro ovládání a čtení periferií.
 */
const uint8_t button = 2;
const uint8_t led = 11;
const uint8_t leftWheelPin = 12;
const uint8_t rightWheelPin = 13;

/**
 * Stavy robota.
 */
unsigned long spiralLastCall;
unsigned long lastStateChange;
boolean firstSpiralCall = true;
boolean turnRight = false;
boolean invertedHintLogic = false;
boolean rightHintEncountered = false;
boolean leftHintEncountered = false;


/**
 * Jednorázová inicializace.
 */
void setup() {
    /* Inicializace výstupu po sériové lince */
    Serial.begin(115200);
    Serial.println("Serial prepared");

    /* Vstupní tlačítko a IR senzory */
    init_button(button);
    init_sensors();

    /* Výstupní LEDka a serva motorů kol */
    pinMode(led, OUTPUT);
    servo_init(leftWheelPin, rightWheelPin);
}

void loop() {
    /* Nastavení reakce na nápovědy podle prvního stisku tlačítka */
    if (check_long_press_button(button)) {
        invertedHintLogic = true;
    }

    /* Vyčkání na stisk tlačítka */
    wait_for_button(button, LOW);


    /* Spuštění sledování čáry */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        /* Přečtení všech nových hodnot */
        read_sensors();

        /* Přenastavení stavu podle nápovědy */
        handle_hints();

        /* Prioritu pravého a levého sensoru určuje stav daný nápovědou */
        if (turnRight) {
            led_on();
            if (second_right()) {
                slightly_right();
                firstSpiralCall = true;
                continue;
            }
            if (second_left() && !middle()) {
                slightly_left();
                firstSpiralCall = true;
                continue;
            }
        } else {
            led_off();
            if (second_left()) {
                slightly_left();
                firstSpiralCall = true;
                continue;
            }
            if (second_right() && !middle()) {
                slightly_right();
                firstSpiralCall = true;
                continue;
            }
        }
        /* Priorita ostatních sensorů je pevně daná */
        if (middle()) {
            half_forward();
            firstSpiralCall = true;
            continue;
        }
        /* Spirálovité prohledávání je až poslední záchranná volba */
        if (spiral()) {
            continue;
        }
    }
#pragma clang diagnostic pop
}

/**
 * Řeší problém hledání nápovědy a nastavení správného stavu pro následující křižovatku.
 */
void handle_hints() {
    /* Po určitou dobu se stav nesmí změnit */
    if (millis() - lastStateChange < STATE_FIXATION) {
        return;
    }

    /* Kontrola nápovědy vlevo */
    if (first_left() && !second_left() && !leftHintEncountered) {
        leftHintEncountered = true;
        rightHintEncountered = false;
    } else if (!first_left() && !second_left() && leftHintEncountered) {
        turnRight = invertedHintLogic;
        lastStateChange = millis();
        leftHintEncountered = false;
        rightHintEncountered = false;
    }

    /* Kontrola nápovědy vpravo */
    if (first_right() && !second_right() && !rightHintEncountered) {
        leftHintEncountered = false;
        rightHintEncountered = true;
    } else if (!first_right() && !second_right() && rightHintEncountered) {
        turnRight = !invertedHintLogic;
        lastStateChange = millis();
        leftHintEncountered = false;
        rightHintEncountered = false;
    }
}

/**
 * Provede krok spirálovitého průzkumu, pokud je funkce volána
 * dostatečně dlouhou dobu (čára je opravdu ztracena).
 *
 * @return Jestli byl spirálovitý průzkum zahájen.
 */
boolean spiral() {
    /* Funkce je volaná poprvé po jiné operaci */
    if (firstSpiralCall) {
        spiralLastCall = millis();
        firstSpiralCall = false;
        return false;
    }

    /* Do uběhnutí definované doby čekáme */
    if (millis() - spiralLastCall < SPIRAL_DELAY) {
        return false;
    }

    left_speed(0.1);
    right_speed(1);

    return true;
}

/**
 * Rozsvítí LEDku.
 */
void led_on() {
    digitalWrite(led, HIGH);
}

/**
 * Zhasne LEDku.
 */
void led_off() {
    digitalWrite(led, LOW);
}