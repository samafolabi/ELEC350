/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#ifndef EXTRAS_H
#define EXTRAS_H

#include "sample_hardware.hpp"
#include "Networkbits.hpp"
#include "string.h"
#include "mbed.h"

class Data {
public:
    time_t datetime;
    float temperature;
    float pressure;
    float light;

    Data(time_t d, float t, float p, float l);
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
