/**
 * Autoři: Geiger & Vašek
 * Projekt: Sledovač čáry s nápovědou u křižovatek
 * Popis:
 *      1) robot je spuštěn stisknutím tlačítka
 *      2) pokud robot nevidí čáru po stanovenou dobu, začne
 *          spirálovité prohledávání
 *      3) robot se snaží po rovné čáře jet plnou rychlostí
 *      4) po aktivaci bočních senzorů uloží nápovědu
 *      5) při aktivaci mezisensorů přikládá prioritu senzoru,
 *          který odpovídá poslední zaznamenané nápovědě
 */
#include <Arduino.h>
#include <Servo.h>

Servo leftWheel;
Servo rightWheel;

#define BLACK           (0)
#define WHITE           (1)
#define NUM_SENSORS     (5)
#define MIN_LEFT_SPEED  (1300)
#define MAX_LEFT_SPEED  (1700)
#define MIN_RIGHT_SPEED (1700)
#define MAX_RIGHT_SPEED (1300)
#define SPIRAL_DELAY    (1000)
#define HINT_DELAY_MAX  (350)
#define HINT_DELAY_MIN  (270)

/**
 * Piny pro ovládání a čtení periferií.
 */
uint8_t button = 2;
uint8_t led = 11;
//uint8_t sensors[] = {3, 4, 5, 6, 7};
uint8_t sensors[] = {A0, A1, A2, A3, A4};
uint8_t leftWheelPin = 12;
uint8_t rightWheelPin = 13;

/**
 * Stavy robota.
 */
unsigned long hintEncounter;
long spiralLastCall = -1;
int sensor_values[] = {0, 0, 0, 0, 0};
boolean turnRight = 0;
boolean rightHintEncountered = 0;
boolean leftHintEncountered = 0;


/**
 * Přečtení a uložení hodnot všech sensorů.
 */
void read_sensors() ;

/**
 * Provede krok spirálovitého průzkumu, pokud je funkce volána
 * dostatečně dlouhou dobu (čára je opravdu ztracena).
 *
 * @return Jestli byl spirálovitý průzkum zahájen.
 */
boolean spiral() ;

/**
 * Určuje, jestli je pod prvním sensorem zleva černá barva.
 *
 * @return Jestli je pod prvním sensorem zleva černá barva.
 */
boolean first_left() ;

/**
 * Určuje, jestli je pod druhým sensorem zleva černá barva.
 *
 * @return Jestli je pod druhým sensorem zleva černá barva.
 */
boolean second_left() ;

/**
 * Určuje, jestli je pod prostředním sensorem černá barva.
 *
 * @return Jestli je pod prostředním sensorem černá barva.
 */
boolean middle() ;

/**
 * Určuje, jestli je pod druhým sensorem zprava černá barva.
 *
 * @return Jestli je pod druhým sensorem zprava černá barva.
 */
boolean second_right() ;

/**
 * Určuje, jestli je pod prvním sensorem zprava černá barva.
 *
 * @return Jestli je pod prvním sensorem zprava černá barva.
 */
boolean first_right() ;

/**
 * Otáčení doleva na místě.
 */
void in_place_left() ;

/**
 * Otáčení doprava na místě.
 */
void in_place_right() ;

/**
 * Dopředná jízda plnou rychlostí.
 */
void full_forward() ;

/**
 * Pomalejší zatáčení doprava obloukem.
 */
void slightly_right() ;

/**
 * Pomalejší zatáčení doleva obloukem.
 */
void slightly_left() ;

/**
 * Jednorázová inicializace.
 */
void setup() {
    /* Inicializace výstupu po sériové lince */
    Serial.begin(115200);
    Serial.println("Serial prepared");


    /* Nastavení vstupů */
    pinMode(button, INPUT);
    /* Tlačítko se zapnutým pull-up -> stisk vyvolá hodnotu LOW */
    digitalWrite(button, HIGH);

    for (int i = 0; i < NUM_SENSORS; i++) {
        pinMode(sensors[i], INPUT);
    }

    /* Nastavení výstupů */
    pinMode(led, OUTPUT);

    leftWheel.attach(leftWheelPin);
    rightWheel.attach(rightWheelPin);
}

void loop() {
    /* Vyčkání na stisk tlačítka */
    int in;
    do {
        in = digitalRead(button);
    } while(in != 0);


    /* Spuštění sledování čáry */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (1) {
        /* Přečtení všech nových hodnot */
        read_sensors();

        /* Přenastavení stavu podle nápovědy */
        if (first_left() && !second_left() && !leftHintEncountered) {
            leftHintEncountered = true;
            rightHintEncountered = false;
            hintEncounter = millis();
        } else if (!first_left() && !second_left()) {
            /* Stav se změní pouze pokud značka nebyla pod senzorem moc dlouho */
//            unsigned long now = millis();
//            unsigned long diff = now - hintEncounter;
//            if (diff < HINT_DELAY_MAX && diff > HINT_DELAY_MIN && leftHintEncountered) {
//                Serial.print("left: ");
//                Serial.println(now - hintEncounter);
            turnRight = false;
//                Serial.println("Left state");
//            }
            leftHintEncountered = false;
        }
        if (first_right() && !second_right() && !rightHintEncountered) {
            leftHintEncountered = false;
            rightHintEncountered = true;
            hintEncounter = millis();
        } else if (!first_right() && !second_right()) {
            /* Stav se změní pouze pokud značka nebyla pod senzorem moc dlouho */
//            unsigned long now = millis();
//            unsigned long diff = now - hintEncounter;
//            if (diff < HINT_DELAY_MAX && diff > HINT_DELAY_MIN && rightHintEncountered) {
//                Serial.print("right: ");
//                Serial.println(now - hintEncounter);
            turnRight = true;
//                Serial.println("Right state");
//            }
            rightHintEncountered = false;
        }

        /* Prioritu pravého a levého sensoru určuje stav daný nápovědou */
        if (turnRight) {
            if (second_right()) {
                slightly_right();
//                in_place_right();
                spiralLastCall = -1;
                continue;
            }
            if (second_left() && !middle()) {
                slightly_left();
//                in_place_left();
                spiralLastCall = -1;
                continue;
            }
        } else {
            if (second_left()) {
                slightly_left();
//                in_place_left();
                spiralLastCall = -1;
                continue;
            }
            if (second_right() && !middle()) {
                slightly_right();
//                in_place_right();
                spiralLastCall = -1;
                continue;
            }
        }
        /* Priorita ostatních sensorů je pevně daná */
        if (middle()) {
            full_forward();
            spiralLastCall = -1;
            continue;
        }
        //TODO: je potřeba na tyto vůbec reagovat jindy než pro nápovědu?
        /*
        if (first_left()) {
            in_place_left();
            spiralLastCall = -1;
            continue;
        }
        if (first_right()) {
            in_place_right();
            spiralLastCall = -1;
            continue;
        }
         */
        if (spiral()) {
            continue;
        }
    }
#pragma clang diagnostic pop
}

/**
 * Přečtení a uložení hodnot všech sensorů.
 */
void read_sensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
//        sensor_values[i] = digitalRead(sensors[i]);
        Serial.print(analogRead(sensors[i]));
        Serial.print(" ");
    }
    Serial.println();
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

/**
 * Určuje, jestli je pod prvním sensorem zleva černá barva.
 *
 * @return Jestli je pod prvním sensorem zleva černá barva.
 */
boolean first_left() {
    return sensor_values[0] == BLACK;
}

/**
 * Určuje, jestli je pod druhým sensorem zleva černá barva.
 *
 * @return Jestli je pod druhým sensorem zleva černá barva.
 */
boolean second_left() {
    return sensor_values[1] == BLACK;
}

/**
 * Určuje, jestli je pod prostředním sensorem černá barva.
 *
 * @return Jestli je pod prostředním sensorem černá barva.
 */
boolean middle() {
    return sensor_values[2] == BLACK;
}

/**
 * Určuje, jestli je pod druhým sensorem zprava černá barva.
 *
 * @return Jestli je pod druhým sensorem zprava černá barva.
 */
boolean second_right() {
    return sensor_values[3] == BLACK;
}

/**
 * Určuje, jestli je pod prvním sensorem zprava černá barva.
 *
 * @return Jestli je pod prvním sensorem zprava černá barva.
 */
boolean first_right() {
    return sensor_values[4] == BLACK;
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
 * Provede krok spirálovitého průzkumu, pokud je funkce volána
 * dostatečně dlouhou dobu (čára je opravdu ztracena).
 *
 * @return Jestli byl spirálovitý průzkum zahájen.
 */
boolean spiral() {
    unsigned long now = millis();
    /* Funkce je volaná poprvé po jiné operaci */
    if (spiralLastCall == -1) {
        spiralLastCall = now;
        return false;
    }
    /* Do uběhnutí definované doby čekáme */
    if (now - spiralLastCall < SPIRAL_DELAY) {
        return false;
    }

    left_speed(0.1);
    right_speed(1);

    return true;
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
    left_speed(0.1);
    right_speed(0.1);
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