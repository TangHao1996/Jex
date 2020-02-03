#include "Request.h"

namespace Jex{

Request::Request() : m_fd(-1), m_req(0), m_handler(NULL){
	
}

Request::Request(int fd, epoll_req_t events, Callback_ptr_t func)
	: m_fd(fd), m_req(events), m_handler(func){

}

Request::~Request(){

}

}
