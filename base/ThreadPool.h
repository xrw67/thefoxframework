
#include <base/Types.h>
#include <base/MutexLock.h>
#include <base/Semaphore.h>
#include <base/Thread.h>

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
		if (_inited)
			return false;
		_inited = true;
		addWorker(threadNum);
		return _inited;
	}
	
	/// @brief 添加线程任务
	void addTask(const ThreadCallback &task)
	{ _tasks.push(task); }
	
	/// @brief 等待线程池结束
	void join()
	{
		for (ThreadVector::const_iterator it = _threads.begin(); 
				it != _threads.end(); ++it)
			(*it)->join();
			
		_threads.clear();
	}
	
	/// @brief 得到线程个数
	size_t size() const { return _thread.size(); }
	
	/// @brief 结束线程池
	void terminate()
	{
		for (ThreadVector::const_iterator it = _threads.begin(); 
				it != _threads.end(); ++it)
			(*it)->stop();
	}

private:
	THEFOX_DISALLOW_EVIL_CONSTRUCTORS(ThreadPool);
	typedef std::vector<std::shared_ptr<Thread>> ThreadVector;
	
	class TaskManager
	{
	public:
		TaskManager() {}
		~TaskManager() {}
		
		void push(const ThreadCallback &task) 
		{
			MutexLockGuard lock(_mutex);
			_tasks.push_back(task);
			_sem.signal();
		}
		ThreadCallback get()
		{
			ThreadCallback task;
			_sem.wait(Semaphore::kInfinite);
			MutexLockGuard lock(_mutex);
			task = _tasks.front();
			_task.pop_front();
			return task;
		}
		
	private:
		THEFOX_DISALLOW_EVIL_CONSTRUCTORS(TaskManager);
		typedef std::vector<ThreadCallback> TaskVector;
		TaskVector _tasks;
		MutexLock _mutex;
		Semaphore _sem;
	};
	
	// 添加工作线程
	void addWorker(int threadNum)
	{
		for (int i = 0; i < threadNum; ++i)
			_threads.push_back(new Thread(taskRunner, "threadpool.taskrunner"));
	}
	
	// 运行任务
	void taskRunner()
	{
		while(true) {
			TaskPtr = _tasks.get()
			if (TaskPtr)
				(*TaskPtr)();
		}
	}
	
	bool _inited;
	ThreadVector _threads;
	TaskManager _tasks;
	static const int32_t kDefaultThreadNum = 10;
};