#include "Channel.h"
#include "EventLoop.h"
#include "network/Socket.h"
#include <unistd.h>
#include <sys/epoll.h>

Channel::Channel(EventLoop* _loop, int _fd) {
	this->loop = _loop;
	this->fd = _fd;
	this->events = 0;
	this->ready = 0;
	this->inEpoll = false;
}

Channel::~Channel() {
	if (this->fd != -1) {
		close(this->fd);
		this->fd = -1;
	}
}

int Channel::getFd() {
	return this->fd;
}

uint32_t Channel::getEvents() {
	return this->events;
}

uint32_t Channel::getReady() {
	return this->ready;
}

bool Channel::getInEpoll() {
	return this->inEpoll;
}

void Channel::setInEpoll(bool _inEpoll) {
	this->inEpoll = _inEpoll;
}

void Channel::setReady(uint32_t _ready) {
	this->ready = _ready;
}

void Channel::enableRead() {
	this->events |= EPOLLIN | EPOLLPRI;
	this->loop->updateChannel(this);
}

void Channel::useET() {
	this->events |= EPOLLET;
	this->loop->updateChannel(this);
}

void Channel::setReadCallback(std::function<void()>_callback) {
	this->readCallback = _callback;
}

void Channel::setWriteCallback(std::function<void()>_callback) {
	this->writeCallback = _callback;
}

void Channel::handleEvent() {
	if (ready & (EPOLLIN | EPOLLPRI)) {
		readCallback();
	}
	if (ready & (EPOLLOUT)) {
		writeCallback();
	}

}