#include "log.h"
#include <iostream>

namespace Jex{
Logger::Logger(const std::string& name):m_name(name){
}

void Logger::log(Level level, LogEvent::ptr event){
	if(level >= m_level){
		for(auto& i:m_appenders){
			i->log(level, event);
		}
	}
}

void Logger::debug(LogEvent::ptr event){
	log(Level::DEBUG, event);
}

void Logger::info(LogEvent::ptr event){
	log(Level::INFO, event);
}

void Logger::warn(LogEvent::ptr event){
	log(Level::WARN, event);
}

void Logger::error(LogEvent::ptr event){
	log(Level::ERROR, event);
}

void Logger::fatal(LogEvent::ptr event){
	log(Level::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender){
	m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender){
	for(auto it= m_appenders.begin();it!=m_appenders.end();it++){
		if(*it == appender){
			m_appenders.erase(it);
			break;
		}
	}
}

void StdoutLogAppender::log(Level level, LogEvent::ptr event) {
	if(level >= m_level){
		std::cout<<m_formatter->format(event);
	}
}

void FileLogAppender::log(Level level, LogEvent::ptr event) {
	if(level >= m_level){
		file_stream<<m_formatter->format(event);
	}
}

void FileLogAppender::reopen(){
	if(file_stream){
		file_stream.close();
	}
	file_stream.open(filename);
}



}

