#pragma once
#include <string>


namespace Jex{	

int create_listenfd(int port);

void setNonBlocking(int fd);

ssize_t readn(int fd, std::string &inBuffer);

}
