#pragma once

#include <sys/epoll.h>
#include <vector>
#include <memory>
#include "Request.h"

#define EPOLL_TIME_INF (-1)

namespace Jex{


class Epoll{
private:
	static const int MAXFDS = 10000;
	int epollfd;
	std::vector<epoll_event> req_events;
	int wait_time;
public:
	Epoll(int time = -1);
	~Epoll();
	void epoll_add(Request::ptr req);
	void epoll_modify(Request::ptr req);
	void epoll_delete(Request::ptr req);
	void poll();

 
};



}
