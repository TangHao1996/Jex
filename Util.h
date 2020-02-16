#pragma once
#include <string>


namespace Jex{	

int create_listenfd(int port);

ssize_t readn(int fd, std::string &inBuffer, bool &ifend);

}
