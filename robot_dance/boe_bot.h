#ifndef boe_bot_state_
#define boe_bot_state_

#include <Arduino.h>
#include <Servo.h>
#include <EEPROM.h>

#include "location.h"


#define MAGIC           ("GV-v001")
#define BLACK           (0)
#define WHITE           (1)
#define NUM_SENSORS     (5)
#define MIN_LEFT_SPEED  (1300)
#define MAX_LEFT_SPEED  (1700)
#define MIN_RIGHT_SPEED (1700)
#define MAX_RIGHT_SPEED (1300)

class boe_bot
{
	Servo leftWheel;
	Servo rightWheel;

	/**
	* Piny pro ovládání a čtení periferií.
	*/
	uint8_t button = 2;
	uint8_t led = 11;
	uint8_t sensors[5] = { 3, 4, 5, 6, 7 };
	uint8_t leftWheelPin = 12;
	uint8_t rightWheelPin = 13;

	/**
	* Stavy robota.
	*/
	int sensor_values[5] = { 0, 0, 0, 0, 0 };
	int current_instruction = 0;

	location location_state;

public:
	
	const location& get_location() const
	{
		return location_state;
	}

	void set_location(const location& loc)
	{
		location_state = loc;
	}

	/**
	* Uloží text do EEPROM od dané adresy o dané délce, případně délku doplní mezerami.
	*
	* @param address Adresa cíle v EEPROM prvního bytu.
	* @param length Délka řetězce.
	* @param text Řetězec k zápisu.
	*/
	void text_to_EEPROM(int address, int length, String text) {
		for (int i = 0; i < text.length(); i++) {
			EEPROM.write(address + i, (byte)text[i]);
		}
		for (int i = text.length(); i < length; i++) {
			EEPROM.write(address + i, ' ');
		}
	}

	/**
	* Přečtení textu o dané délce z EEPROM.
	*
	* @param address Adresa EEPROM začátku řetězce.
	* @param length Délka řetězce.
	* @return Přečtený řetězec oříznutý o mezery na konci.
	*/
	String text_from_EEPROM(int address, int length) {
		String text = "";
		for (int i = 0; i < length; i++) {
			char character = (char)EEPROM.read(address + i);
			text = text + character;
		}
		while (text[text.length() - 1] == ' ') {
			text = text.substring(0, text.length() - 1);
		}
		return text;
	}

	/**
	* Zápis integeru do EEPROM.
	*
	* @param address Adresa cíle prvního bytu integeru v EEPROM.
	* @param value Hodnota integeru k zápisu.
	*/
	void int_to_EEPROM(int address, int value) {
		byte byte1 = value;
		byte byte2 = value >> 8;
		EEPROM.write(address, byte1);
		EEPROM.write(address + 1, byte2);
	}

	/**
	* Načtení integeru z EEPROM.
	*
	* @param address Adresa prvního bytu integeru v EEPROM.
	* @return Přečtená hodnota integeru.
	*/
	int int_from_EEPROM(int address) {
		return EEPROM.read(address) + ((EEPROM.read(address + 1)) << 8);
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
		leftWheel.writeMicroseconds((int)((MAX_LEFT_SPEED - MIN_LEFT_SPEED) * (speed + 1) / 2.0 + MIN_LEFT_SPEED));
	}

	/**
	* Přeškáluje hodnotu z [-1; 1] a spustí pravé servo odpovídajícím směrem a rychlostí.
	* -1 je maximum vzad, 0 stop, 1 maximum vpřed.
	*
	* @param speed Rychlost pravého kola z intervalu [-1; 1], speciálně 0 znamená zastavení.
	*/
	void right_speed(double speed) {
		rightWheel.writeMicroseconds((int)((MAX_RIGHT_SPEED - MIN_RIGHT_SPEED) * (speed + 1) / 2.0 + MIN_RIGHT_SPEED));
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

	/**
	* Jednorázová inicializace.
	*/
	void setup() {
		/* Inicializace výstupu po sériové lince */
		Serial.begin(9600);
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

	void start()
	{
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
					text_to_EEPROM(address, sizeof(MAGIC), MAGIC);
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
