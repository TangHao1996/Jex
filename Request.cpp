#include "Request.h"
#include <sys/epoll.h>
#include <functional>
#include <iostream>
#include "Epoll.h"
#include <unistd.h>

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
	close(m_fd);
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
	

	auto poll_ = m_poll.lock();
	if(!m_sess)
		return;//listen request

	if(m_sess->get_outBuffer_size() > 0){
		std::cout<<m_sess->get_outBuffer_size()<<std::endl;
		poll_->epoll_modify(m_fd, EPOLLET|EPOLLIN|EPOLLOUT);
		m_event = EPOLLET|EPOLLIN|EPOLLOUT;
	}
	else{
		poll_->epoll_modify(m_fd, EPOLLET|EPOLLIN);	
		m_event = EPOLLET|EPOLLIN;
	}
}

void Request::mod_event(){
	std::shared_ptr<Epoll> poll_ = m_poll.lock();
	if(m_ready_event & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
		poll_->epoll_modify(m_fd, EPOLLOUT|EPOLLET);
	else if(m_ready_event & EPOLLOUT){
		poll_->epoll_modify(m_fd, EPOLLIN|EPOLLET);
	}
	m_ready_event = __uint32_t(0);
}




}
