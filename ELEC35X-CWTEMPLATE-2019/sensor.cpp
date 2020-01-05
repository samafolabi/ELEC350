/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#include "sensor.h"
#include "mbed.h"

#define SAMPLE 1

float SAMPLE_RATE_S = 5;
EventQueue queue;
Thread sensor_thread;
bool started = false;
Ticker sample_timer;

void sensor_read() {
		while(true) {
				ThisThread::flags_wait_all(SAMPLE);
				float temp = sensor.getTemperature();
				float pres = sensor.getPressure();
				float ldr = adcIn.read();
				Data data(time(NULL), temp, pres, ldr);
				data.print(true);
		}
}

void sampling(float sample_rate) {
		sample_timer.detach();
		SAMPLE_RATE_S = sample_rate;
		sample_timer.attach(&sample_interrupt, SAMPLE_RATE_S);
}

void sampling_setup() {
		sensor_thread.start(sensor_read);
		sample_timer.attach(&sample_interrupt, SAMPLE_RATE_S);
}

void sample_interrupt() {
		sensor_thread.flags_set(SAMPLE);
}
