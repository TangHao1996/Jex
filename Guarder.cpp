#include "Guarder.h"
#include "Util.h"
namespace Jex{

Guarder::Guarder(int port)
	:listenfd(create_listenfd(port))
	m_poll(new Epoll()){
	
	
}

Guarder::~Guarder(){

}

void Guarder::loop(){
	
}


}
