#pragma once
#include <pthread.h>
#include <vector>
#include <queue>
#include <memory>


namespace Jex {

const int MAX_THREADS = 1024;
const int MIN_THREADS = 8;
const int THREAD_STEP = 8;

struct task{
	void (*func)(void *);
	void *args;
};

class ThreadPool{
		
private:
	static pthread_mutex_t lock;
	static pthread_cond_t wait_task_cond;
	static bool shutdown; 

	static std::vector<pthread_t> threads;
	static std::queue<task*> task_queue;
	static int m_queue_size;

	static void *thread_process(void *args);
public:
	static int create(int num_threads, int task_queue_size);
	static int add_threads(int add_num);
	static int destroy();
};

	
}
