#pragma once

#include<stdint.h>

namespace Jex{

class Request {
public:
	typedef __uint32_t epoll_req_t;
	typedef void(*Callback_ptr_t)();//回调函数指针

	Request(int fd, epoll_req_t events, Callback_ptr_t func);
	~Request();

	int getfd() {return m_fd;}
	__uint32_t get_epoll_events() {return m_req;}
	void set_handler(Callback_ptr_t func) {m_handler = func;}
private:
	int m_fd;
	epoll_req_t m_req;
	Callback_ptr_t m_handler;
};

}
