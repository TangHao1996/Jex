#pragma once

#include <stdint.h>
#include <memory>

namespace Jex{

class Request {
public:
	typedef std::shared_ptr<Request> ptr;
	typedef __uint32_t epoll_req_t;//epoll_event.events
	typedef void(*Callback_ptr_t)();//回调函数指针

	Request(int fd, epoll_req_t events, Callback_ptr_t func);
	~Request();

	int getfd() {return m_fd;}
	__uint32_t get_epoll_events() {return m_req;}
	void set_handler(Callback_ptr_t func) {m_handler = func;}
	void setfd(int fd) {m_fd = fd;}
	void set_epoll_req(epoll_req_t epoll_req){m_req = epoll_req;}
private:
	int m_fd;
	epoll_req_t m_req;
	Callback_ptr_t m_handler;
};

}
