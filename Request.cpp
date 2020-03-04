#include "Request.h"
#include <sys/epoll.h>
#include <functional>
#include <iostream>

namespace Jex{

void handle_sess_read(HttpSession::ptr& sess){
	sess->readHandler();
}

void handle_sess_write(HttpSession::ptr& sess){
	sess->writeHandler();
}

Request::Request() : m_fd(-1), m_event(0),m_ready_event(0), m_read_handler(NULL), m_write_handler(NULL){
	
}

Request::~Request(){
	std::cout<<"distruct request, fd: "<<m_fd<<std::endl;
}

void Request::bind_session(HttpSession::ptr sess){
	set_read_handler(std::bind(&handle_sess_read, sess));
	set_write_handler(std::bind(&handle_sess_write, sess));
	m_sess = sess;
	sess->setfd(m_fd);
}

void Request::handle_epoll(){
	m_event = 0;
	if((m_ready_event & EPOLLHUP) && !(m_ready_event & EPOLLIN)){
		return;
	}
	if(m_ready_event & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)){
		if(m_read_handler)
			m_read_handler();
	}
	if(m_ready_event & EPOLLOUT){
		if(m_write_handler)
			m_write_handler();
	}
}

}
