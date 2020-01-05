/* UoP ELEC350 Low Power Environmental Sensor - Group S - Samson Afolabi*/

#include "serial.h"
#include "mbed.h"


Serial pc(USBTX, USBRX);

void serial_setup() {
	pc.baud(9600);
  serial_thread.start(serial_comm);
}

void serial_comm() {
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
								sampling(val);
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
