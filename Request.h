#pragma once

#include <stdint.h>
#include <memory>
#include <functional>
#include "HttpSession.h"
//#include "Epoll.h"
namespace Jex{

class Epoll;
	
class Request {
public:
	typedef std::shared_ptr<Request> ptr;
	typedef __uint32_t epoll_req_t;//epoll_event.events
	typedef std::function<void()> Callback_ptr_t;//回调函数指针

   
	//Request(int fd, epoll_req_t event, Callback_ptr_t func);
	Request();
	~Request();

	int getfd() {return m_fd;}
	__uint32_t get_epoll_events() {return m_event;}
	void set_read_handler(Callback_ptr_t func) {m_read_handler = func;}
	void set_write_handler(Callback_ptr_t func) {m_write_handler = func;}
	void set_post_handler(Callback_ptr_t func){m_post_handler = func;}
	void setfd(int fd) {m_fd = fd;}
	void set_epoll(std::shared_ptr<Epoll> poll){m_poll = poll;}
	void set_epoll_event(epoll_req_t epoll_event){m_event = epoll_event;}
	void set_ready_event(epoll_req_t event){m_ready_event = event;}
	void bind_session(HttpSession::ptr sess);

	void handle_epoll();
	void mod_event();
	bool if_disconnect(){return m_sess->if_disconnect();}
private:
	int m_fd;
	epoll_req_t m_event;
	epoll_req_t m_ready_event;
	HttpSession::ptr m_sess;
	std::weak_ptr<Epoll> m_poll;
	Callback_ptr_t m_read_handler;
	Callback_ptr_t m_write_handler;
	Callback_ptr_t m_post_handler;
};

}
