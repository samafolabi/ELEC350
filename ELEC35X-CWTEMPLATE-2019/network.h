#ifndef NETWORK_H
#define NETWORK_H

#include "extras.h"
#include "mbed.h"

extern TCPSocket srv;
extern TCPSocket *clt_sock;
extern EthernetInterface eth;

extern void server();
extern void network_setup();

#endif
