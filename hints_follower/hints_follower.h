
#ifndef HINTS_FOLLOWER_H
#define HINTS_FOLLOWER_H

#include <Arduino.h>


/**
 * Řeší problém hledání nápovědy a nastavení správného stavu pro následující křižovatku.
 */
void handle_hints() ;

/**
 * Rozsvítí LEDku.
 */
void led_on() ;

/**
 * Zhasne LEDku.
 */
void led_off() ;

/**
 * Provede krok spirálovitého průzkumu, pokud je funkce volána
 * dostatečně dlouhou dobu (čára je opravdu ztracena).
 *
 * @return Jestli byl spirálovitý průzkum zahájen.
 */
boolean spiral() ;


#endif //HINTS_FOLLOWER_H
