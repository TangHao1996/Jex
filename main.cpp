//#include"log.h"
#include "ThreadPool.h"
#include "Epoll.h"
#include "Guarder.h"
#include "signal.h"
#include "Log.h"

using namespace Jex;

Guarder *test_server;

void quit_handler(int a){
	test_server->stop();
	Logger::get_ins("", 0).reset();
}

int main(){
 
  	Logger::get_ins("", 0)->init();	
	LOG<<"log test";
	signal(SIGINT, quit_handler);	
	test_server = new Guarder(8888);
	ThreadPool::create(8, 1000);
	test_server->start();
	test_server->loop();
	delete test_server;

	return 0;
}


