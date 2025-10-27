#pragma once
#include <functional>
class Socket;
class EventLoop;

// 事件通道类，封装文件描述符和事件回调
class Channel
{
private:
	EventLoop* loop;  // 事件循环
	int fd;  // 文件描述符

	uint32_t events;  //channel 事件类型
	uint32_t ready;  // channel 就绪事件类型

	bool inEpoll;  // 是否挂载到epoll

	std::function<void()> readCallback;  // 读回调函数
	std::function<void()> writeCallback;  // 写回调函数
public:
	/**
	* @brief Channel类用于封装文件描述符和事件回调，与EventLoop配合使用。
	* @param _loop 指向EventLoop对象的指针，用于管理事件循环。
	* @param _fd 文件描述符，表示需要监听的通道。 
	*/
	Channel(EventLoop* _loop, int _fd);

	~Channel();

	/*
	* @brief 获取文件描述符
	* @return 文件描述符
	*/
	int getFd();

	/*
	* @brief 获取事件类型
	* @return 事件类型
	*/
	uint32_t getEvents();

	/*
	* @brief 获取就绪事件类型
	* @return 就绪事件类型
	*/
	uint32_t getReady();

	/*
	* @brief 判断channel是否已添加到epoll
	* @return 如果已添加则返回true，否则返回false
	*/
	bool getInEpoll();

	/*
	* @brief 设置channel是否添加到epoll
	* @param _inEpoll
	*/
	void setInEpoll(bool _inEpoll);

	/*
	* @brief 处理就绪事件，调用相应的回调函数
	*/
	void handleEvent();

	/*
	* @brief 启用读事件监听
	*/
	void enableRead();

	/*
	* @brief 启用边缘模式
	*/
	void useET();

	/*
	* @brief 设置就绪事件类型
	* @param _ready 就绪事件类型
	*/
	void setReady(uint32_t _ready);

	/*
	* @brief 设置读回调函数
	* @param _callback 读回调函数
	*/
	void setReadCallback(std::function<void()>_callback);

	/*
	* @brief 设置写回调函数
	* @param _callback 写回调函数
	*/
	void setWriteCallback(std::function<void()>_callback);
};

