#pragma once

#include <sys/epoll.h>
#include <vector>
#include <memory>

namespace Jex{


class Epoll{
private:
	static const int MAXFDS = 10000;
	int epollfd;
	std::vector<epoll_event> req_events;
	int wait_time;
public:
	Epoll(int time);
	~Epoll();
	void epoll_add(int fd, epoll_event* event);
	void epoll_modify(int fd, epoll_event* event);
	void epoll_delete(int fd, epoll_event* event);
	void poll();


};



}
