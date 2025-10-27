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
		std::vector<ChannelPtr>chs;
		// 获取触发事件通道
		// 每个Channel对象代表一个事件源（如一个套接字）
		// 逐个处理事件
		chs = this->epollPoller->poll();
		for (auto it = chs.begin(); it != chs.end(); ++it)
		{
			(*it)->handleEvent();
		}
	}
}

void EventLoop::updateChannel(const ChannelPtr& channel) {
	this->epollPoller->updateChannels(channel);
}