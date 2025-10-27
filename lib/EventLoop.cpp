#include "EventLoop.h"
#include "EpollPoller.h"
#include "Channel.h"
#include <vector>

EventLoop::EventLoop() {
	this->epollPoller = new EpollPoller();
	this->quit = false;
}

EventLoop::~EventLoop() {
	delete this->epollPoller;
}

void EventLoop::loop() {
	while (!this->quit)
	{
		std::vector<Channel*>chs;
		chs = this->epollPoller->poll();
		for (auto it = chs.begin(); it != chs.end(); ++it)
		{
			(*it)->handleEvent();
		}
	}
}

void EventLoop::updateChannel(Channel* channel) {
	this->epollPoller->updateChannels(channel);
}