/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#include "extras.h"
#include "mbed.h"

Data::Data(time_t d, double t, double p, double l) {
		datetime = d;
		temperature = t;
		pressure = p;
		light = l;
}

void Data::print(bool lcd_print) {
		if (lcd_print) {
				lcd.cls();
				lcd.printf("T:%5.1f", this->temperature);
				lcd.locate(0,1);
				lcd.printf("P:%5.1f", this->pressure);
				lcd.printf("L:%4.3f", this->light);
		}
		pc.printf("Temperature: %5.1f\n", this->temperature);
		pc.printf("Pressure: %5.1f\n", this->pressure);
		pc.printf("LDR: %f\n", this->light);
		
		pc.printf("Time: %s\n", ctime(&this->datetime));
}

Thread net_thread, serial_thread;
Mutex data_lock;
Data data(time(NULL), 100.0, 25.6, 376.3);

//section from internet
const std::string WHITESPACE = " \n\r\t\f\v";

std::string ltrim(const std::string& s)
{
	size_t start = s.find_first_not_of(WHITESPACE);
	return (start == std::string::npos) ? "" : s.substr(start);
}

std::string rtrim(const std::string& s)
{
	size_t end = s.find_last_not_of(WHITESPACE);
	return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

std::string trim(const std::string& s)
{
	return rtrim(ltrim(s));
}
//section over
