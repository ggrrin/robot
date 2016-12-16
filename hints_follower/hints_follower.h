
#ifndef ROBOT_FOLLOWER_HINTS_HITS_FOLLOWER_HPP
#define ROBOT_FOLLOWER_HINTS_HITS_FOLLOWER_HPP

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


#endif //ROBOT_FOLLOWER_HINTS_HITS_FOLLOWER_HPP
