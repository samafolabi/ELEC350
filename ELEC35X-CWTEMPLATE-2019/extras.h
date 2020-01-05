/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#ifndef EXTRAS_H
#define EXTRAS_H

#include "sample_hardware.hpp"
#include "Networkbits.hpp"
#include <ctime>
#include "string.h"
#include "mbed.h"

class Data {
public:
    time_t datetime;
    double temperature;
    double pressure;
    double light;

    Data(time_t d, double t, double p, double l);
		void print(bool lcd_print = false);
};

extern Serial pc;
extern Thread net_thread;
extern Thread serial_thread;
extern Mutex data_lock;
extern Data data;

extern std::string ltrim(const std::string& s);

extern std::string rtrim(const std::string& s);

extern std::string trim(const std::string& s);

#endif
