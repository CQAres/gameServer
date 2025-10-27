#pragma once
#include <functional>

class EpollPoller;
class Channel;

using ChannelPtr = std::shared_ptr<Channel>;  // 智能指针

// 事件循环类
class EventLoop
{
private:
	EpollPoller* epollPoller;  //epoll事件循环
	bool quit;  // 退出标志

public:
	EventLoop();
	~EventLoop();

	/*
	* 
	*/
	void loop();

	/*
	* epoll channel
	*/
	void updateChannel(const ChannelPtr& channel);

	/*
	* 
	*/
	void addThread(std::function<void()>);
};

