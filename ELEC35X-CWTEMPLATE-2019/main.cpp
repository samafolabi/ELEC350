#include "sample_hardware.hpp"
#include "Networkbits.hpp"
#include "string.h"

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


Serial pc(USBTX, USBRX);
Thread network, serial;
TCPSocket srv;
Mutex data_lock;
Data data(time(NULL), 100.0, 25.6, 376.3);

void server() {
    TCPSocket *clt_sock;
    while (true) {
        clt_sock=srv.accept();
			
        data_lock.lock();
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
			
				clt_sock->send(response.c_str(), response.size());

        clt_sock->close();
        ThisThread::sleep_for(1000);
    }
}

void s_comm() {
    while(true) {
        char c;
        char size = 20, count = 0;
        char cmd0[size];
        do {
						//bug where if you backspace, it doesnt delete the character
						//backspace counts as a character
						while (!pc.readable()){}
            c = pc.getc();
            cmd0[count] = toupper(c);
            count++;
        } while (c != '\n' && c != '\r' && count < 19);
				cmd0[count] = '\0';
        string cmd(cmd0);
        cmd = trim(cmd);

        if (cmd == "READ NOW") {
            data_lock.lock();
            pc.printf("%s %6.2f %6.2f %6.2f\r\n", ctime(&data.datetime),
                        data.temperature, data.pressure, data.light);
            data_lock.unlock();
        } else if (cmd == "READ BUFFER") {
            pc.printf("%s\r\n", cmd.c_str());
        } else if (cmd.substr(0,4) == "SET ") {
            double val = atof(cmd.substr(4).c_str());
            if (val >= 0.1 && val <= 30.0) {
                pc.printf("T UPDATED TO %4.1f\n",val);
            }
            else {pc.puts("Out of Range Error");}
        } else if (cmd.substr(0,6) == "STATE ") {
            if (cmd.substr(6) == "ON") {pc.puts("Sampling on");}
            else if (cmd.substr(6) == "OFF") {pc.puts("Sampling off");}
            else {pc.puts("Input error");}
        } else if (cmd.substr(0,8) == "LOGGING ") {
            if (cmd.substr(8) == "ON") {pc.puts("Logging on");}
            else if (cmd.substr(8) == "OFF") {pc.puts("Logging off");}
            else {pc.puts("Input error");}
        } else if (cmd == "FLUSH") {
            pc.printf("%s\r\n", cmd.c_str());
        } else if (cmd == "EJECT") {
            pc.printf("%s\r\n", cmd.c_str());
        } else {
            pc.puts("Invalid command");
        }
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
        } else {
            pc.printf("Listen error=%d\n\r",err);
        }
    } else {
        pc.printf("Bind error=%d\n\r",err);
    }
		serial.start(s_comm);
	while(true){
		ThisThread::sleep_for(2000);
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

    
