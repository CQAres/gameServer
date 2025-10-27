#pragma once
#include <sys/epoll.h>
#include <vector>

class Channel;
/**
 * Epoll轮询类
 * 封装epoll的增删改查和事件监听
 */
class EpollPoller
{
private:
	int fd;  // epoll文件描述符
	struct epoll_event* events;  // epoll下的事件

public:
	/*
	* @brief 创建一个epoll实例
	*/
	EpollPoller();

	/*
	* @brief 释放资源
	*/
	~EpollPoller();

	/*
	* @brief 更新事件通道
	* @param channel需要更新的事件通道指针
	*/
	void updateChannels(Channel* _channel);

	/*
    * @brief 删除事件通道
    * @param channel 需要删除的事件通道指针
	*/
	void deleteChannels(Channel* _channel);

	/*
    * @brief 轮询事件
    * @param timeout 超时时间（毫秒），默认为-1（阻塞等待）
    * @return 返回就绪的事件通道列表
	*/
	std::vector<Channel*> poll(int timeout = -1);
};

