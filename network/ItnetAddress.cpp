#include "ItnetAddress.h"
#include <strings.h>

ItnetAddress::ItnetAddress() {
	bzero(&addr, sizeof(addr));
}

ItnetAddress::ItnetAddress(const char* _ip, uint16_t _port) {
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(_ip);
	addr.sin_port = htons(_port);
}

ItnetAddress::~ItnetAddress() {

}

sockaddr_in ItnetAddress::getAddr() {
	return this->addr;
}

char* ItnetAddress::getIp() {
	return inet_ntoa(addr.sin_addr);
}

uint16_t ItnetAddress::getPort() {
	return ntohs(addr.sin_port);
}

void ItnetAddress::setInetAddr(sockaddr_in _addr) {
	addr = _addr;
}