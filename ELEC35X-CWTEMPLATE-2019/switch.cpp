/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#include "switch.h"
#include "mbed.h"

#define P_LENGTH 7

const int P[] = { 0,3,6,16,19,22,25 };
const char* TEXT[] = {"MONTH", "DAY", "YEAR", "HOUR", "MINUTE", "SECOND", "OK"};

SwitchDate d;

void SwitchDate::time_to_lcd() {
	this->month = tim->tm_mon+1;
	this->day = tim->tm_mday;
	this->year = tim->tm_year+1900;
	this->hour = tim->tm_hour;
	this->minute = tim->tm_min;
	this->second = tim->tm_sec;
}

void SwitchDate::lcd_to_time() {
	tim->tm_mon = this->month-1;
	tim->tm_mday = this->day;
	tim->tm_year = this->year-1900;
	tim->tm_hour = this->hour;
	tim->tm_min = this->minute;
	tim->tm_sec = this->second;
}

short SwitchDate::time_change(char detail, bool direction) {
	char max = 31;
	switch (detail) {
		case 0: //MONTH
			if (direction) {this->month += this->month == 12 ? -11 : 1;}
			else {this->month -= this->month == 1 ? -11 : 1;}
			if (this->month == 2) {this->day = this->day > 28 ? 28 : this->day;}
			else if (this->month == 4 || this->month == 6 
							|| this->month == 9 || this->month == 11) {this->day = this->day > 30 ? 30 : this->day;}
			return this->month;
			break;
		case 1: //DAY
			if (this->month == 2) {max = 28;}
			else if (this->month == 4 || this->month == 6 
							|| this->month == 9 || this->month == 11) {max = 30;}
			if (direction) {this->day += this->day == max ? 1-max : 1;}
			else {this->day -= this->day == 1 ? 1-max : 1;}
			return this->day;
			break;
		case 2: //YEAR
			if (direction) {this->year++;}
			else {this->year -= this->year == 1900 ? 1900 : 1;}
			return this->year;
			break;
		case 3: //HOUR
			if (direction) {this->hour += this->hour == 23 ? -23 : 1;}
			else {this->hour -= this->hour == 0 ? -23 : 1;}
			return this->hour;
			break;
		case 4: //MINUTE
			if (direction) {this->minute += this->minute == 59 ? -59 : 1;}
			else {this->minute -= this->minute == 0 ? -59 : 1;}
			return this->minute;
			break;
		case 5: //SECOND
			if (direction) {this->second += this->second == 59 ? -59 : 1;}
			else {this->second -= this->second == 0 ? -59 : 1;}
			return this->second;
			break;
	}
	
	return -1;
}

void press(DigitalIn& button) {
	while(button==0){}
	ThisThread::sleep_for(200);
	while(button==1){}
	ThisThread::sleep_for(200);
}

void press2(DigitalIn& button) {
	ThisThread::sleep_for(200);
	while(button==1){}
	ThisThread::sleep_for(200);
}

void locate(short col, short row) {
	short cmd = 0x80;
	row /= 16;
	if (row == 1) {cmd += 0x40;}
	col %= 16;
	cmd += col;
	lcd.writeCommand(cmd);
	lcd.locate(col, row);
}

void lcd_date_setup() {
		lcd.cls();
		lcd.writeCommand(0x0D);
		locate(0,0);
		//sets an initial time
		time_t old = time(NULL);
		d.tim = localtime(&old);
		d.tim->tm_hour=15;
		d.tim->tm_mday=7;
		d.tim->tm_year=120;
		set_time(mktime(d.tim));
}

void lcd_date() {
			DigitalIn& blue = onBoardSwitch;
			lcd.printf("Hey");
			press(blue);
			lcd.cls();
			//gets current time
			time_t timm = time(NULL);
			d.tim = localtime(&timm);
			d.time_to_lcd();
			lcd.printf("%02d %02d %04d\n"\
			"%02d:%02d:%02d %7.7s", d.month,
			d.day, d.year, d.hour,
			d.minute, d.second, d.text.c_str());
			char pointer = 0;
			char location = P[pointer];
			locate(location+1,location+1);
			while (true) {
				if (SW1 == 1) {
					press2(SW1);
					pointer = pointer >= 6 ? 0 : pointer + 1;
					location = P[pointer];
					if (pointer == 2) {locate(location+3,location+3);}
					else if (pointer == 6) {locate(31,31);}
					else {locate(location+1,location+1);}
				}
				
				if (SW2 == 1) {
					press2(SW2);
					pointer = pointer <= 0 ? 6 : pointer - 1;
					location = P[pointer];
					if (pointer == 2) {locate(location+3,location+3);}
					else if (pointer == 6) {locate(31,31);}
					else {locate(location+1,location+1);}
				}
					
				if (blue == 1) {
					press2(blue);
					char detail = 0;
					
					if (location == P[6]) {
						d.lcd_to_time();
						set_time(mktime(d.tim));
						pc.printf("%s", asctime(d.tim));
						
						lcd.cls();
						lcd.printf("Time Set!");
						ThisThread::sleep_for(1000);
						lcd.cls();
						break;
					}
					
					if (location == P[0]) { detail = 0; }
					else if (location == P[1]) { detail = 1; }
					else if (location == P[2]) { detail = 2; }
					else if (location == P[3]) { detail = 3; }
					else if (location == P[4]) { detail = 4; }
					else if (location == P[5]) { detail = 5; }
					else {lcd.cls(); lcd.printf("Error"); break;}
						locate(P[6],P[6]);
						lcd.printf("%7.7s", TEXT[detail]);
						locate((location)+1,location);
						
						while (true) {
							if (SW1 == 1 || SW2 == 1) {
								bool direction;
								if (SW1 == 1) { direction = true; press2(SW1); }
								else { direction = false; press2(SW2); }
								short result = d.time_change(detail,direction);
								if (result < 0) {lcd.cls(); lcd.printf("Error"); break;}
								//pc.printf("%d %d %d %d\n", detail, P[detail], location, result);
								//location = P[(location+1) % P_LENGTH];
								if (detail == 0) {
									locate(P[0],P[0]);
									lcd.printf("%02d", result);
									locate(P[1],P[1]);
									lcd.printf("%02d", d.day);
								} else if (detail == 2) {
									locate(P[2],P[2]);
									lcd.printf("%04d", result);
								} else {
									locate(P[detail],P[detail]);
									lcd.printf("%02d", result);
								}
								locate(location+1,location);
							}
							
							if (blue == 1) {
								press2(SW2);
								locate(P[6],P[6]);
								lcd.printf("%7.7s", TEXT[6]);
								locate(location,location);
								break;
							}
						}
						
					}
				
			}
}
