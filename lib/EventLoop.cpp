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
	threadId = std::this_thread::get_id();
	while (!this->quit)
	{
		// 1、执行IO事件
		std::vector<Channel*>chs;
		chs = this->epollPoller->poll();
		for (auto it = chs.begin(); it != chs.end(); ++it)
		{
			(*it)->handleEvent();
		}

		// 2、执行跨线程任务队列事件
		std::queue<std::function<void()>> tasks;
        {
            std::lock_guard<std::mutex> lock(taskMtx);
            tasks.swap(taskQueue); // 交换，减少锁时间
        }
        while (!tasks.empty()) {
            tasks.front();
            tasks.pop();
        }
	}
}

void EventLoop::updateChannel(Channel* channel) {
	this->epollPoller->updateChannels(channel);
}

void EventLoop::queueInLoop(std::function<void()> cb) {
    {
        std::lock_guard<std::mutex> lock(taskMtx);
        taskQueue.push(std::move(cb));
    }
}

bool EventLoop::isInLoopThread() const {
    return std::this_thread::get_id() == threadId;
}