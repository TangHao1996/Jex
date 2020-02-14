//#include"log.h"
#include "ThreadPool.h"
#include "Epoll.h"
#include "Guarder.h"

using namespace Jex;
int main(){

	Guarder test_server(8888);
	ThreadPool::create(8, 1000);
	test_server.start();
	test_server.loop();




	return 0;
}
