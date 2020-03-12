#include "Log.h"
#include <iostream>
#include <string.h>
#include <pthread.h>

const char LOG_FILE_PATH[] = "Jex.log";

namespace Jex{

void LogBuffer::append(const char *line, size_t len){
	if(cur + len > buf + m_size){
		std::cout<<"buffer has no enough space."<<std::endl;
		return;
	}
	memcpy(cur, line, len);
	cur = cur + len;
}

void LogBuffer::flush(FILE *file){
	size_t ret_len = fwrite(buf, sizeof(char), (cur - buf), file);
	if(ret_len != static_cast<size_t>(cur - buf)){
		std::cout<<"fwrite error,"<<std::endl;
	}
	cur = buf;
	fflush(file);
}


Logger::Logger(): fp(nullptr), quit(true){

}

Logger::~Logger(){

	quit = true;
	if(pthread_join(flush_tid, NULL) != 0){
		perror("flush thread join error");
	}
	while(!free_queue.empty()){
		free_queue.pop();
	}
	while(!flush_queue.empty()){
		flush_queue.pop();
	}

	if(fp){
		fclose(fp);
	}
}

void Logger::init(){
	fp = fopen(LOG_FILE_PATH, "w+");
	if(fp == nullptr){
		std::cout<<"open log file error."<<std::endl;
	}
	if(pthread_create(&flush_tid, NULL, flush_buffer, (void*)this) < 0){
		perror("flush thread create error");
		return;
	}
}

void Logger::append(const char *file, int line, const char *logline, size_t len){
	//TO DO
}


void *Logger::flush_buffer(void *ptr){
	Logger *logger_ = (Logger*)ptr;
	logger_->flush_buffer_run();
	return (void *)0;
}

void Logger::flush_buffer_run(){
	LogBuffer::ptr cur;
	while(true){
		std::unique_lock<std::mutex> lock(flush_mtx);
		while(!quit && flush_queue.empty()){
			cond.wait(lock);
		}

		if(flush_queue.empty() && quit)//只有队列为空且Logger退出时才结束此线程
			return;

		cur = flush_queue.front();
		flush_queue.pop();
		cur->flush(fp);
		{
		std::lock_guard<std::mutex> lock(free_mtx);
		free_queue.push(cur);
		}
	}

}





}
