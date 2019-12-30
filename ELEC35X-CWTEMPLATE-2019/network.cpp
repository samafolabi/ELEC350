/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#include "network.h"
#include "mbed.h"

#define HTTP_STATUS_LINE "HTTP/1.0 200 OK"
#define HTTP_HEADER_FIELDS "Content-Type: text/html; charset=utf-8"
#define HTTP_HEAD1 "" \
"<!DOCTYPE html>" \
"<html>" \
"<head><title> Group S Environmental Sensor </title>"
#define HTTP_SCRIPT1 "<script>" "\r\n" \
"function send() {"\
"		var dat = document.getElementById('date').valueAsNumber/1000;" "\r\n" \
"		var tim = document.getElementById('time').value;" "\r\n" \
"		dat += (parseInt(tim.substr(0,2))*60*60) + (parseInt(tim.substr(3))*60);" "\r\n" \
"		var xhttp = new XMLHttpRequest();" "\r\n" \
"		xhttp.onreadystatechange = function() {" "\r\n" \
"			if (this.readyState == 4 && this.status == 200) {" "\r\n" \
"				alert(this.responseText);" "\r\n" \
"			}" "\r\n" \
"		};" "\r\n" \
"		xhttp.open('POST', '"
#define HTTP_SCRIPT2 "', true);" "\r\n" \
"		xhttp.send('Group_S_Date_Time:' + dat.toString());" "\r\n" \
"}" "\r\n" \
"</script>" "\r\n"
#define HTTP_HEAD2 "</head>" "\r\n"
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
"						<input type='date' value='2020-01-07' min='1900-01-01' id='date'>" "\r\n" \
"						<input type='time' value='15:00' id='time'>" "\r\n" \
"						<button id='send' onclick='send()'>SEND</button>" "\r\n" \
"       </div>" "\r\n"                                          \
"   </body>" "\r\n"                                             \
"</html>" "\r\n"
#define IP        "10.0.0.10"
#define NET   "255.255.255.0"
#define GATEWAY   "10.0.0.1"
#define RESPONSE HTTP_STATUS_LINE "\r\n"   \
                 HTTP_HEADER_FIELDS "\r\n" \
                 "\r\n"                    \
                 HTTP_HEAD1 "\r\n" \
                 HTTP_SCRIPT1 IP HTTP_SCRIPT2 "\r\n" \
                 HTTP_HEAD2 "\r\n"

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
				char buf[2048];
				int len = clt_sock->recv(buf, 2048);
				buf[len] = '\0';
			
				string res(buf);
			
				string response = "";
				size_t check = res.find("POST");
				if (check != string::npos) {
						string div = "Group_S_Date_Time:";
						size_t d = res.find(div) + div.length();
						string data = trim(res.substr(d));
						time_t t = std::atol(data.c_str());
						struct tm *t2 = localtime(&t);
						set_time(mktime(t2));
						response = "SENT";
						pc.printf("%s\n", asctime(t2));
				} else {
						data_lock.lock();
						response = RESPONSE;
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
				}
				clt_sock->send(response.c_str(), response.size());

        clt_sock->close();
        ThisThread::sleep_for(1000);
    }
}
