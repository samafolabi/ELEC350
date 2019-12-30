/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#include "network.h"
#include "mbed.h"

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

TCPSocket srv;
TCPSocket *clt_sock;
EthernetInterface eth;

void network_setup() {
		pc.puts("Connecting...");
		
    eth.set_network(IP, NET, GATEWAY);
    eth.connect();
    pc.printf("IP address is '%s'\r\n", eth.get_ip_address());

    srv.open(&eth);
    int err=srv.bind(eth.get_ip_address(), 80);
    
    if(err==0) {
        err=srv.listen(5);
        if(err==0) {
            net_thread.start(server);
        } else {
            pc.printf("Listen error=%d\n\r",err);
        }
    } else {
        pc.printf("Bind error=%d\n\r",err);
    }
}

void server() {
    
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
