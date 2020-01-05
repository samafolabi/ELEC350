/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#ifndef SENSOR_H
#define SENSOR_H

#include "extras.h"
#include "mbed.h"
#include <ctime>



extern float SAMPLE_RATE_S;
extern double SAMPLING;
extern EventQueue queue;
extern Thread sensor_thread;
extern bool started;
extern Ticker sample_timer;

extern void sensor_read();
extern void sampling(float sample_rate);
extern void sampling_setup();
extern void sample_interrupt();

#endif
