#include "sample_hardware.hpp"
#include "Networkbits.hpp"
#include "string.h"
#include <ctime>
#include "extras.h"
#include "switch.h"
#include "serial.h"
#include "network.h"

int main()
{
		serial_setup();
		network_setup();
		lcd_date_setup();
	
		while(true){
			lcd_date();
		};
    
}

/*
//Threads
Thread nwrkThread;
 

int main() {
    //Greeting
    printf("Testing\n\n");    
    
    //Power on self test
    post();
    
    //Initialise the SD card (this needs to move)
    if ( sd.init() != 0) {
        printf("Init failed \n");
        lcd.cls();
				lcd.locate(0,0);
        lcd.printf("CANNOT INIT SD");        
        errorCode(FATAL);
    } 
    
    //Create a filing system for SD Card
    FATFileSystem fs("sd", &sd);     

    //Open to WRITE
    FILE* fp = fopen("/sd/test.csv","a");
    if (fp == NULL) {
        error("Could not open file for write\n");
        lcd.cls();
        lcd.printf("CANNOT OPEN FILE\n\n");
        errorCode(FATAL);
    }
    
    //Last message before sampling begins
    lcd.cls();
    lcd.printf("READY\n\n");
        
        
    //Press either switch to unmount
    while ((SW1 == 0) && (SW2 == 0)) {
        
        //Base loop delay
        wait(1.0);
        
        //Read environmental sensors
        double temp = sensor.getTemperature();
        double pressure = sensor.getPressure();
        
        //Write new data to LCD (not fast!)
        lcd.cls();
        lcd.printf("Temp   Pressure\n"); 
        lcd.printf("%6.1f ",temp);
        lcd.printf("%.2f\n",pressure);
        
        //Write to SD (potentially slow)
        fprintf(fp, "%6.1f,%.2f\n\r", temp, pressure);
    }
    
    //Close File
    fclose(fp);
    
    //Close down
    sd.deinit();
    printf("Unmounted...\n");
    lcd.cls();
    lcd.printf("Unmounted...\n\n");
    
    //Flash to indicate goodness
    while(true) {
        greenLED = 1;
        wait(0.5);
        greenLED = 0;
        wait(0.1);    
    }
}
*/

    
