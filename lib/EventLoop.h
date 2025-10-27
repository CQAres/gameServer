#pragma once
#include <functional>

class EpollPoller;
class Channel;

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
	* @brief 事件循环
	*/
	void loop();

	/*
	* @brief 更新Channel
	* @param channel 需要更新的事件通道指针
	*/
	void updateChannel(Channel* channel);

	/*
	* @brief 添加线程
	* @param function 线程函数
	*/
	void addThread(std::function<void()>);
};

