//#include"log.h"
#include "ThreadPool.h"
#include "Epoll.h"
#include "Guarder.h"
#include "signal.h"
#include "Log.h"

using namespace Jex;

Guarder::ptr test_server;

void quit_handler(int a){
	test_server->stop();
	//Logger::get_ins("", 0).reset();
	ThreadPool::destroy();
}

int main(){
 
  	//Logger::get_ins("", 0)->init();	
	//LOG<<"log test";
	signal(SIGINT, quit_handler);	
	test_server.reset(new Guarder(80));
	ThreadPool::create(8, 1000);
	test_server->start();
	test_server->loop();


	return 0;
}


