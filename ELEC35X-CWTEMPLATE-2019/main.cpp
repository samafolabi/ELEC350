#include "sample_hardware.hpp"
#include "Networkbits.hpp"
#include "string.h"

// This is a very short demo that demonstrates all the hardware used in the coursework.
// You will need a network connection set up (covered elsewhere). The host PC should have the address 10.0.0.1


#define HTTP_STATUS_LINE "HTTP/1.0 200 OK"
#define HTTP_HEADER_FIELDS "Content-Type: text/html; charset=utf-8"
#define HTTP_TITLE "" \
"<!DOCTYPE html>" \
"<html>" \
"<head><title> Group S Environmental Sensor </title></head>" "\r\n"

#define BODY1 ""   \
"   <body style=\"display:flex;text-align:center\">" "\r\n"     \
"       <div style=\"margin:auto\">" "\r\n"                     \
"           <p>Date/Time: "

#define BODY2 "</p>" "\r\n"                        \
"           <h3>Sensor Data</h3>" "\r\n"                        \
"           <p>Temperature: "

#define BODY3 "deg C</p>" "\r\n"                   \
"           <p>Pressure: "

#define BODY4 "mbar</p>" "\r\n"                       \
"           <p>Light Level: "

#define BODY5 "</p>" "\r\n"                        \
"       </div>" "\r\n"                                          \
"   </body>" "\r\n"                                             \
"</html>" "\r\n"

#define RESPONSE HTTP_STATUS_LINE "\r\n"   \
                      HTTP_HEADER_FIELDS "\r\n" \
                      "\r\n"                    \
                      HTTP_TITLE "\r\n"

#define IP        "10.0.0.10"
#define NET   "255.255.255.0"
#define GATEWAY   "10.0.0.1"

class Data {
public:
    time_t datetime;
    float temperature;
    float pressure;
    float light;

    Data(time_t d, float t, float p, float l) {
        datetime = d;
        temperature = t;
        pressure = p;
        light = l;
    }
};

Serial pc(USBTX, USBRX);
Thread network;
TCPSocket srv;
Mutex data_lock;
Data data(time(NULL), 100.0, 25.6, 376.3);

void server() {
    TCPSocket *clt_sock;
    while (true) {
        pc.puts("Hey");
        clt_sock=srv.accept();
        pc.puts("Hey");
        data_lock.lock();
        pc.puts("Hey");
        string response = RESPONSE;
				response += BODY1;
				response += ctime(&data.datetime);
				response += BODY2;
				response += to_string(data.temperature);
				response += BODY3;
				response += to_string(data.pressure);
				response += BODY4;
				response += to_string(data.light);
				response += BODY5;
				response += "\r\n";
        data_lock.unlock();
        pc.puts("Hey");
			clt_sock->send(response.c_str(), response.size());
			
        pc.puts("Hey");

        clt_sock->close();
        ThisThread::sleep_for(1000);
    }
}

int main()
{
		pc.baud(9600);
    EthernetInterface eth;
    eth.set_network(IP, NET, GATEWAY);
    eth.connect();
    pc.printf("IP address is '%s'\r\n", eth.get_ip_address());

    srv.open(&eth);
    int err=srv.bind(eth.get_ip_address(), 80);
    
    if(err==0) {
        err=srv.listen(5);
        if(err==0) {
            network.start(server);
					//server();
        } else {
            pc.printf("Listen error=%d\n\r",err);
        }
    } else {
        pc.printf("Bind error=%d\n\r",err);
    }
    pc.puts("Heyyiiiiop");
	while(true){
	
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

    
