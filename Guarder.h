#pragma once

#include <memory>
#include "ThreadPool.h"
#include "Epoll.h"
#include "Request.h"

namespace Jex {

class Guarder{
public:
	Guarder(int port);
	~Guarder();

	void start();
	void loop();
	void stop();
private:
	int listenfd;
	int port;
	Epoll::ptr m_poll;
	Request::ptr listen_req;
	//std::vector<Request::ptr> all_req;
	bool quit;

	void connect_handler();
};


}
