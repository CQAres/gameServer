#pragma once
#include <functional>
#include <queue>
#include <mutex>
#include <thread>

class EpollPoller;
class Channel;

// 事件循环类
class EventLoop
{
private:
	EpollPoller* epollPoller;  //epoll事件循环
	bool quit;  // 退出标志

	std::queue<std::function<void()>> taskQueue; //跨线程任务队列
    std::mutex taskMtx;
    std::thread::id threadId; // 保存 loop 所在线程 id
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

	/**
	* @brief 将任务添加到跨线程任务队列中
	* @param cb 任务函数
	*/
	void queueInLoop(std::function<void()> cb);

	/**
	* @brief 判断当前线程是否为事件循环所在的线程
	*/
    bool isInLoopThread() const;
};

