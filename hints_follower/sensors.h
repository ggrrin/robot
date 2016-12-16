
#ifndef ROBOT_FOLLOWER_HINTS_SENSORS_H
#define ROBOT_FOLLOWER_HINTS_SENSORS_H

#include <Arduino.h>

#define BLACK           (0)
#define WHITE           (1)
#define NUM_SENSORS     (5)


int sensor_values[] = {0, 0, 0, 0, 0};
const uint8_t sensors[] = {3, 4, 5, 6, 7};


/**
 * Inicializace pinů s připojenými senzory.
 */
void init_sensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        pinMode(sensors[i], INPUT);
    }

}

/**
 * Přečtení a uložení hodnot všech sensorů.
 */
void read_sensors() {
    for (int i = 0; i < NUM_SENSORS; i++) {
        sensor_values[i] = digitalRead(sensors[i]);
    }
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

#endif //ROBOT_FOLLOWER_HINTS_SENSORS_H
