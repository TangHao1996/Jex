#include "ThreadPool.h"

namespace Jex {

pthread_mutex_t ThreadPool::lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t ThreadPool::ThreadPool::wait_task_cond = PTHREAD_COND_INITIALIZER;
bool ThreadPool::shutdown = false;
std::vector<pthread_t> ThreadPool::threads;
std::queue<task_t> ThreadPool::task_queue;
int ThreadPool::ThreadPool::m_queue_size = 0;



int ThreadPool::create(int num_threads, int queue_size){
	int num = 0;
	if(num_threads <= MAX_THREADS && num_threads >= MIN_THREADS)
		num = num_threads;
	else
		num = MIN_THREADS;

	m_queue_size = queue_size;

	for(int i=0;i<num;i++){
		if(threads.size() >= MAX_THREADS)
			return 0;

		pthread_t thread_;
		if(pthread_create(&thread_, NULL,thread_process, (void *)(0)) != 0){
			perror("thread create error");
			return -1;
		}
		threads.push_back(thread_);
	}
	
	return 0;
}
	
int ThreadPool::add_threads(int add_num){
	int num = add_num > THREAD_STEP? add_num : THREAD_STEP;

	for(int i=0;i<num;i++){
		if(threads.size() >= MAX_THREADS)
			return 0;

		pthread_t thread_;
		if(pthread_create(&thread_, NULL,thread_process, (void *)(0)) != 0){
			perror("thread create error");
			return -1;
		}
		threads.push_back(thread_);
	}
	return 0;
}

int ThreadPool::destroy(){
	shutdown = true;
	
	pthread_mutex_lock(&lock);
	if(pthread_cond_broadcast(&wait_task_cond) != 0 || pthread_mutex_unlock(&lock) != 0){
		perror("pthread unlock error");
		return -1;
	}

	for(int i=0; i<threads.size(); i++){
		if(pthread_join(threads[i], NULL) != 0){
			perror("thread join error");
			return -1;
		}
	}
	//所有线程结束后
	pthread_mutex_lock(&lock);
	pthread_mutex_destroy(&lock);
	pthread_cond_destroy(&wait_task_cond);
	
	return 0;
}

void* ThreadPool::thread_process(void *args){
	while(true){
		task_t task;
		pthread_mutex_lock(&lock);
		if(shutdown)
			return NULL;
		while(task_queue.empty() && !shutdown){
			pthread_cond_wait(&wait_task_cond, &lock);//解锁等待
		}
		//返回加锁
		task.func = task_queue.front().func;
		task.args = task_queue.front().args;
		task_queue.pop();
		pthread_mutex_unlock(&lock);
		(*(task.func))(task.args);
	}
	pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
	return NULL;
}

	
}
