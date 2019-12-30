#include "sample_hardware.hpp"
#include "Networkbits.hpp"
#include "string.h"
#include <ctime>

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
#define P_LENGTH 7
const int P[] = { 0,3,6,16,19,22,25 };

struct {
			short month = 0;
			short day = 1;
			short year = 0;
			short hour = 0;
			short minute = 0;
			short second = 0;
			string text = "OK";
		} lcd_time;

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

void time_to_lcd(struct tm* tim) {
	lcd_time.month = tim->tm_mon+1;
	lcd_time.day = tim->tm_mday;
	lcd_time.year = tim->tm_year+1900;
	lcd_time.hour = tim->tm_hour;
	lcd_time.minute = tim->tm_min;
	lcd_time.second = tim->tm_sec;
}

void lcd_to_time(struct tm* tim) {
	tim->tm_mon = lcd_time.month-1;
	tim->tm_mday = lcd_time.day;
	tim->tm_year = lcd_time.year-1900;
	tim->tm_hour = lcd_time.hour;
	tim->tm_min = lcd_time.minute;
	tim->tm_sec = lcd_time.second;
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
short time_change(char detail, bool direction) {
	char max = 31;
	switch (detail) {
		case 0: //MONTH
			if (direction) {lcd_time.month += lcd_time.month == 12 ? -11 : 1;}
			else {lcd_time.month -= lcd_time.month == 1 ? -11 : 1;}
			if (lcd_time.month == 2) {lcd_time.day = lcd_time.day > 28 ? 28 : lcd_time.day;}
			else if (lcd_time.month == 4 || lcd_time.month == 6 
							|| lcd_time.month == 9 || lcd_time.month == 11) {lcd_time.day = lcd_time.day > 30 ? 30 : lcd_time.day;}
			locate(P[1],P[1]);
			lcd.printf("%02d", lcd_time.day);
			return lcd_time.month;
			break;
		case 1: //DAY
			if (lcd_time.month == 2) {max = 28;}
			else if (lcd_time.month == 4 || lcd_time.month == 6 
							|| lcd_time.month == 9 || lcd_time.month == 11) {max = 30;}
			if (direction) {lcd_time.day += lcd_time.day == max ? 1-max : 1;}
			else {lcd_time.day -= lcd_time.day == 1 ? 1-max : 1;}
			return lcd_time.day;
			break;
		case 2: //YEAR
			if (direction) {lcd_time.year++;}
			else {lcd_time.year -= lcd_time.year == 1900 ? 1900 : 1;}
			return lcd_time.year;
			break;
		case 3: //HOUR
			if (direction) {lcd_time.hour += lcd_time.hour == 23 ? -23 : 1;}
			else {lcd_time.hour -= lcd_time.hour == 0 ? -23 : 1;}
			return lcd_time.hour;
			break;
		case 4: //MINUTE
			if (direction) {lcd_time.minute += lcd_time.minute == 59 ? -59 : 1;}
			else {lcd_time.minute -= lcd_time.minute == 0 ? -59 : 1;}
			return lcd_time.minute;
			break;
		case 5: //SECOND
			if (direction) {lcd_time.second += lcd_time.second == 59 ? -59 : 1;}
			else {lcd_time.second -= lcd_time.second == 0 ? -59 : 1;}
			return lcd_time.second;
			break;
	}
	
	return -1;
}
const char* TEXT[] = {"MONTH", "DAY", "YEAR", "HOUR", "MINUTE", "SECOND", "OK"};

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
    /*EthernetInterface eth;
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
		serial.start(s_comm);*/
		lcd.cls();
		lcd.writeCommand(0x0D);
		locate(0,0);
		time_t timm;
		time_t old = time(NULL);
		struct tm *tim = localtime(&old);
		tim->tm_hour=15;
		tim->tm_mday=7;
		tim->tm_year=120;
		set_time(mktime(tim));
		DigitalIn& blue = onBoardSwitch;
	
		while(true){
			//ThisThread::sleep_for(2000);
			lcd.printf("Hey");
			press(blue);
			lcd.cls();
			timm = time(NULL);
			tim = localtime(&timm);
			time_to_lcd(tim);
			lcd.printf("%02d %02d %04d\n"\
			"%02d:%02d:%02d %7.7s", lcd_time.month,
			lcd_time.day, lcd_time.year, lcd_time.hour,
			lcd_time.minute, lcd_time.second, lcd_time.text.c_str());
			char pointer = 0;
			char location = P[pointer];
			locate(location,0);
			while (true) {
				if (SW1 == 1) {
					press2(SW1);
					pointer = pointer >= 6 ? 0 : pointer + 1;
					location = P[pointer];
					locate(location,location);
				}
				
				if (SW2 == 1) {
					press2(SW2);
					pointer = pointer <= 0 ? 6 : pointer - 1;
					location = P[pointer];
					locate(location,location);
				}
					
				if (blue == 1) {
					press2(blue);
					char detail = 0;
					
					if (location == P[6]) {
						lcd_to_time(tim);
						set_time(mktime(tim));
						pc.printf("%s", asctime(tim));
						
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
								short result = time_change(detail,direction);
								if (result < 0) {lcd.cls(); lcd.printf("Error"); break;}
								pc.printf("%d %d %d %d\n", detail, P[detail], location, result);
								//location = P[(location+1) % P_LENGTH];
								locate(P[detail],P[detail]);
								if (detail == 2) {lcd.printf("%04d", result);}
								else {lcd.printf("%02d", result);}
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

    
