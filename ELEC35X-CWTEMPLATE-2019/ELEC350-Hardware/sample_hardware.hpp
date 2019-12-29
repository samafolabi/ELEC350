#ifndef __sample_hardware__
#define __sample_hardware__

//#define BME
#ifdef BME
#include "BME280.h"
#else
#include "BMP280.h"
#endif
#include "TextLCD.h"
#include "SDBlockDevice.h"
#include "FATFileSystem.h"

enum ELEC350_ERROR_CODE {OK, FATAL};

extern DigitalOut onBoardLED;
extern DigitalOut redLED;
extern DigitalOut yellowLED;
extern DigitalOut greenLED;

extern DigitalIn  onBoardSwitch;
extern DigitalIn  SW1;
extern DigitalIn  SW2;
//extern Serial pc;
extern AnalogIn adcIn;

#ifdef BME
extern BME280 sensor;
#else
extern BMP280 sensor;
#endif

extern TextLCD lcd;
extern SDBlockDevice sd;

extern void post();
extern void errorCode(ELEC350_ERROR_CODE err);

#endif