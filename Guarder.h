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
	void loop();
private:
	int listenfd;
	int port;
	std::shared_ptr<Epoll> m_poll;
	Request::ptr listen_req;
};


}
