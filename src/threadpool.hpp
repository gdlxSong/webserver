#ifndef GDL_THREAD_POOL_H
#define GDL_THREAD_POOL_H


#include<list>
#include <vector>
#include <queue>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include"util.hpp"
#include"SyncQueue.hpp"





namespace gdl
{
#define  MAX_THREAD_NUM 256
	//�̳߳�,�����ύ��κ�������ķ����ʽ����������ִ��,���Ի�ȡִ�з���ֵ
	//��֧�����Ա����, ֧���ྲ̬��Ա������ȫ�ֺ���,Opteron()������
	class threadpool : noncopyable
	{
		using Task = std::function<void()>;
		// �̳߳�
		std::vector<std::thread> pool;
		// �������
		std::queue<Task> tasks;
		// ͬ��
		std::mutex m_lock;
		// ��������
		std::condition_variable cv_task;
		// �Ƿ�ر��ύ
		std::atomic<bool> stoped;
		//�����߳�����
		std::atomic<int>  idlThrNum;
	public:
		inline threadpool(int size = std::thread::hardware_concurrency()) :stoped{ false }
		{
			idlThrNum = size < 1 ? 1 : size;
			for (size = 0; size < idlThrNum; ++size)
			{   //��ʼ���߳�����
				pool.emplace_back(
					[this]
					{ // �����̺߳���
						while (!this->stoped)
						{
							std::function<void()> task;
							{   // ��ȡһ����ִ�е� task
								std::unique_lock<std::mutex> lock{ this->m_lock };// unique_lock ��� lock_guard �ĺô��ǣ�������ʱ unlock() �� lock()
								this->cv_task.wait(lock,
									[this] {
										return this->stoped.load() || !this->tasks.empty();
									}
								); // wait ֱ���� task
								if (this->stoped && this->tasks.empty())
									return;
								task = std::move(this->tasks.front()); // ȡһ�� task
								this->tasks.pop();
							}
							idlThrNum--;
							task();
							idlThrNum++;
						}
					}
					);
			}
		}
		inline ~threadpool()
		{
			stoped.store(true);
			cv_task.notify_all(); // ���������߳�ִ��
			for (std::thread& thread : pool) {
				//thread.detach(); // ���̡߳���������
				if (thread.joinable())
					thread.join(); // �ȴ���������� ǰ�᣺�߳�һ����ִ����
			}
		}
	public:
		// �ύһ������
		// ����.get()��ȡ����ֵ��ȴ�����ִ����,��ȡ����ֵ
		// �����ַ�������ʵ�ֵ������Ա��
		// һ����ʹ��   bind�� .commit(std::bind(&Dog::sayHello, &dog));
		// һ������ mem_fn�� .commit(std::mem_fn(&Dog::sayHello), &dog)
		template<class F, class... Args>
		auto commit(F&& f, Args&& ... args) ->std::future<decltype(f(args...))>
		{
			if (stoped.load())    // stop == true ??
				throw std::runtime_error("commit on ThreadPool is stopped.");
			using RetType = decltype(f(args...)); // typename std::result_of<F(Args...)>::type, ���� f �ķ���ֵ����
			auto task = std::make_shared<std::packaged_task<RetType()> >(
				std::bind(std::forward<F>(f), std::forward<Args>(args)...)
				);    // wtf !
			std::future<RetType> future = task->get_future();
			{    // ������񵽶���
				std::lock_guard<std::mutex> lock{ m_lock };//�Ե�ǰ���������  lock_guard �� mutex �� stack ��װ�࣬�����ʱ�� lock()��������ʱ�� unlock()
				tasks.emplace(
					[task]()
					{ // push(Task{...})
						(*task)();
					}
				);
			}
			cv_task.notify_one(); // ����һ���߳�ִ��
			return future;
		}
		//�����߳�����
		int idlCount() { return idlThrNum; }
	};




	class ThreadPool : gdl::noncopyable {


		using Task = std::function<void()>;
	public:
		ThreadPool(int nThread = std::thread::hardware_concurrency()) : exited(false), idleThreads(0){
			start(nThread);
		}
		~ThreadPool() {
			stop();
		}

		void start(int nThread) {

			for (int i = 0; i < nThread; i++) {
				if(threadgroups.size() < MAX_THREAD_NUM)
					threadgroups.emplace_back(std::make_shared<std::thread>(&ThreadPool::handleThread, this));
			}
		}

		void handleThread() {

			Task task;
			while (!exited) {
				if (tasks.pop(task)) {
					idleThreads--;
					task();			
					idleThreads++;
				}

			}
		}

		void commit(Task&& task) {

			tasks.push(std::forward<Task>(task));
		}


		void stopThreadgroups() {
			exited = true;
			tasks.stop();
		}

		void stop() {
			std::call_once(flag, [=] {stopThreadgroups(); });//=this
		}

		int idleThread() { return idleThreads; }

	private:
		std::atomic<bool> exited;
		std::once_flag flag;
		std::atomic<int> idleThreads;
		gdl::SyncQueue<Task> tasks;
		std::list<std::shared_ptr<std::thread> > threadgroups;
	};




}





#endif