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
	int wait_time;
public:
	typedef std::shared_ptr<Epoll> ptr;
	static const int MAX_REQS;	

	Epoll(int time = -1);
	~Epoll();
	void epoll_add(Request::ptr req);
	void epoll_modify(Request::ptr req);
	void epoll_delete(Request::ptr req, Request::epoll_req_t);
	void poll();
	void getReadyRequest(std::vector<Request::ptr> &ready_requests);
 
};



}
