/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#ifndef SWITCH_H
#define SWITCH_H

#include "extras.h"
#include "mbed.h"
#include <ctime>

class SwitchDate {
	public:
		short month = 0;
		short day = 1;
		short year = 1900;
		short hour = 0;
		short minute = 0;
		short second = 0;
		string text = "OK";
		struct tm *tim;
	
		void time_to_lcd();
		void lcd_to_time();
		short time_change(char detail, bool direction);
};

extern SwitchDate d;

extern void press(DigitalIn& button);
extern void press2(DigitalIn& button);
extern void locate(short col, short row);
extern void lcd_date_setup();
extern void lcd_date();



#endif
