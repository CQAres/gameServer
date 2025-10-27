#include "EpollPoller.h"
#include "util/util.h"
#include "Channel.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1000

EpollPoller::EpollPoller() {
	this->fd = epoll_create1(0);
	ifError(this->fd == -1, "epoll create error!");
	this->events = new epoll_event[MAX_EVENTS];
	bzero(events, sizeof(*events) * MAX_EVENTS);
}

EpollPoller::~EpollPoller() {
    if (this->fd != -1) {
        close(this->fd);
        this->fd = -1;
    }
    delete [] this->events;
}

std::vector<Channel*> EpollPoller::poll(int timeout) {
    std::vector<Channel*> activeChannels;
    // 用epoll_wait等待事件并返回活跃的Channel列表
    int nfds = epoll_wait(this->fd, this->events, MAX_EVENTS, timeout);
    ifError(nfds == -1, "epoll wait error");
    for (int i = 0; i < nfds; ++i) {
        Channel *ch = (Channel*)events[i].data.ptr;
        ch->setReady(events[i].events);
        activeChannels.push_back(ch);
    }
    return activeChannels;
}

void EpollPoller::updateChannels(Channel* channel) {
    if (channel == nullptr) {
        ifError(true, "EpollPoller::updateChannels channel is nullptr");
    }
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    int fd = channel->getFd();
    ev.events = channel->getEvents();
    ev.data.ptr = channel;
    if (!channel->getInEpoll()) {
        ifError(epoll_ctl(this->fd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        channel->setInEpoll(true);
    }
    else {
        ifError(epoll_ctl(this->fd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll modify error");
    }
}

void EpollPoller::deleteChannels(Channel* channel) {
    if (channel == nullptr) {
        ifError(true, "EpollPoller::deleteChannels channel is nullptr");
    }
    int fd = channel->getFd();
    if (fd < 0) {
        ifError(true, "EpollPoller::deleteChannels invalid file descriptor");
    }
    ifError(epoll_ctl(this->fd, EPOLL_CTL_DEL, fd, NULL) == -1, "epoll delete error");
    channel->setInEpoll(false);
}