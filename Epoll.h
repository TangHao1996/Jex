#pragma once

#include <sys/epoll.h>
#include <vector>
#include <memory>
#include "Request.h"

#define EPOLL_TIME_INF (-1)

namespace Jex{


class Epoll{
private:
	int epollfd;
	std::vector<epoll_event> ready_events;
	std::vector<Request::ptr> all_req;//use fd as index to request
	int wait_time;
public:
	typedef std::shared_ptr<Epoll> ptr;
	static const int MAX_REQS;	

	Epoll(int time = -1);
	~Epoll();
	void epoll_add(Request::ptr req);
	void epoll_modify(int fd, __uint32_t events);
	void epoll_delete(int fd);
	int poll();
	void getReadyRequest(std::vector<Request::ptr> &ready_requests, int events_cnt);
};



}
