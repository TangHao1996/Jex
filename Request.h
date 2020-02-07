#pragma once

#include <stdint.h>
#include <memory>
#include <functional>

namespace Jex{

class Request {
public:
	typedef std::shared_ptr<Request> ptr;
	typedef __uint32_t epoll_req_t;//epoll_event.events
	typedef std::function<void()> Callback_ptr_t;//回调函数指针

   
	Request(int fd, epoll_req_t event, Callback_ptr_t func);
	Request();
	~Request();

	int getfd() {return m_fd;}
	__uint32_t get_epoll_events() {return m_event;}
	void set_handler(Callback_ptr_t func) {m_handler = func;}
	void setfd(int fd) {m_fd = fd;}
	void set_epoll_event(epoll_req_t epoll_event){m_event = epoll_event;}
	void set_ready_event(epoll_req_t event){m_ready_event = event;}

	void handle_epoll(){m_handler();};

private:
	int m_fd;
	epoll_req_t m_event;
	epoll_req_t m_ready_event;
	Callback_ptr_t m_handler;
};

}
