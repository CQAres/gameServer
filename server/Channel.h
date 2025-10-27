#pragma once
#include <functional>
class Socket;
class EventLoop;

class Channel
{
private:
	EventLoop* loop;  // 
	int fd;  // channel

	uint32_t events;  //channel
	uint32_t ready;  //

	bool inEpoll;  // channel

	std::function<void()> readCallback;  //
	std::function<void()> writeCallback;  //
public:
	Channel(EventLoop* _loop, int _fd);
	~Channel();

	/*
	* channel
	*/
	int getFd();

	/*
	* channel
	*/
	uint32_t getEvents();

	/*
	* channel
	*/
	uint32_t getReady();

	/*
	* channel
	*/
	bool getInEpoll();

	/*
	* channel epoll
	*/
	void setInEpoll(bool _inEpoll);

	/*
	* channel
	*/
	void handleEvent();

	/*
	* channel
	*/
	void enableRead();

	/*
	* channel
	*/
	void useET();

	/*
	* channel
	*/
	void setReady(uint32_t _ready);

	/*
	* 
	*/
	void setReadCallback(std::function<void()>_callback);

	/*
	* 
	*/
	void setWriteCallback(std::function<void()>_callback);
};

