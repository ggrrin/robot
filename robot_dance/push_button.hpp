
#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <Arduino.h>

#define DEBOUNCE_TIME   (50)
#define LONG_PRESS_TIME (500)


/**
 * Class providing basic operations on button.
 */
class push_button {

    /**
     * Button connector pin.
     */
    const uint8_t button_pin = 2;

public:

    /**
     * Initialize button pin.
     */
    void init_button();

    /**
     * Sets the interrupt service routine for a button push.
     */
    void attach_ISR_on_push(void (*ISR)(void)) const;

    /**
     * Clears the interrupt service routine for a button push.
     */
    void detach_ISR_on_push() const;

    /**
     * Gets value on the button pin.
     *
     * @return The value on the button pin.
     */
    int read_button() const;

    /**
     * Gets if the button is currently pushed down.
     *
     * @return If the button is currently pushed down.
     */
    bool is_pushed() const;

    /**
     * Actively awaits desired value of the button pin.
     * Provides debounce technique to get rid of noise.
     *
     * @param desiredValue Desired value on the button pin.
     */
    void wait_for_button(const int desiredValue) const;

    /**
     * Actively awaits button push.
     */
    void wait_for_button_push() const;

    /**
     * Actively awaits button release.
     */
    void wait_for_button_release() const;

    /**
     * Actively awaits press of the button.
     *
     * @return If the press was long.
     */
    boolean check_long_press_button() const;

};



//class push_button

inline void push_button::init_button() {
    /* Button with pull-up resistor -> LOW value means push */
    pinMode(button_pin, INPUT);
    digitalWrite(button_pin, HIGH);
}

inline void push_button::attach_ISR_on_push(void (*ISR)(void)) const {
    attachInterrupt((uint8_t) digitalPinToInterrupt(button_pin), ISR, LOW);
}

inline void push_button::detach_ISR_on_push() const {
    detachInterrupt((uint8_t) digitalPinToInterrupt(button_pin));
}

inline int push_button::read_button() const {
    return digitalRead(button_pin);
}

inline bool push_button::is_pushed() const {
    return read_button() == LOW;
}

void push_button::wait_for_button(const int desiredValue) const {
    int in1, in2;
    do {
        in1 = read_button();
        delay(DEBOUNCE_TIME);
        in2 = read_button();
    } while (in1 != desiredValue || in1 != in2);
}

void push_button::wait_for_button_push() const {
    wait_for_button(LOW);
}

void push_button::wait_for_button_release() const {
    wait_for_button(HIGH);
}

boolean push_button::check_long_press_button() const {
    wait_for_button_push();
    unsigned long pressStart = millis();
    wait_for_button_release();

    return (millis() - pressStart) > LONG_PRESS_TIME;
}

#endif //PUSH_BUTTON_HPP
