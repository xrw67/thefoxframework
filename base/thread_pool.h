#ifndef _THEFOX_BASE_THREADPOOL_H_
#define _THEFOX_BASE_THREADPOOL_H_

#include <queue>
#include <base/common.h>
#include <base/mutex.h>
#include <base/semaphore.h>
#include <base/thread.h>

namespace thefox
{

typedef std::function<void()> TaskMethod;

class ThreadPool
{
public:
	ThreadPool()
		: _inited(false)
	{}
	~ThreadPool()
	{}
	
	/// @brief 初始化线程池
	/// @param[in] threadNum 线程个数
	bool init(int threadNum = kDefaultThreadNum)
	{
		if (_inited) {
			return false;
		}
		_inited = true;
		addWorker(threadNum);
		return _inited;
	}
	
	/// @brief 添加线程任务
	void addTask(const TaskMethod &task)
	{ _tasks.put(task); }
	
	/// @brief 等待线程池结束
	void join()
	{
		for (ThreadVector::const_iterator it = _threads.begin(); it != _threads.end(); ++it) {
			(*it)->join();
		}
			
		_threads.clear();
	}
	
	/// @brief 得到线程个数
	size_t size() const { return _threads.size(); }
	
	/// @brief 结束线程池
	void terminate()
	{
		for (ThreadVector::const_iterator it = _threads.begin(); it != _threads.end(); ++it) {
			(*it)->stop();
		}
	}

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(ThreadPool);
	typedef std::vector<std::shared_ptr<Thread>> ThreadVector;
	
	class TaskQueue
	{
	public:
		TaskQueue() {}
		~TaskQueue() {}
		
		void put(const TaskMethod &task) 
		{
			MutexLockGuard lock(_mutex);
			_tasks.push(task);
			_sem.signal();
		}
		ThreadCallback get()
		{
			TaskMethod task;
			_sem.wait(Semaphore::kInfinite);
			MutexLockGuard lock(_mutex);
			task = _tasks.front();
			_tasks.pop();
			return task;
		}
		
	private:
		THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TaskQueue);
		typedef std::queue<TaskMethod> Tasks;
		Tasks _tasks;
		MutexLock _mutex;
		Semaphore _sem;
	};
	
	// 添加工作线程
	void addWorker(int threadNum)
	{
		for (int i = 0; i < threadNum; ++i) {
			std::shared_ptr<Thread> thread(
				new Thread(std::bind(&ThreadPool::taskRunner, this),
								"threadpool.taskrunner"));
			_threads.push_back(thread);					
			thread->start();
		}
	}
	
	// 运行任务
	void taskRunner()
	{
		while(true) {
			TaskMethod task = _tasks.get();
			task();
		}
	}
	
	bool _inited;
	ThreadVector _threads;
	TaskQueue _tasks;

	static const int32_t kDefaultThreadNum = 10;
};

} // namespace thefox

#endif // _THEFOX_BASE_THREADPOOL_H_