
/*

	#title: Object pool.
	@desc: ����أ��Գػ���˼�������ٶ����������ͷŲ���.
	@author: gdl.
	@version: 0.0.1
	@date: 2019.10.15 22:43
	@modify: $date
	@reference: ������Ӧ��c++11�� - ���� - p238.

*/





#ifndef OBJECTPOOL_GDL_HPP
#define OBJECTPOOL_GDL_HPP

//�������ƣ�
/*
	@����
		1. ����ؿ��Խ������⹹�캯��.
		2. ����ش�С���Զ�̬����.

	@ʹ��Ҫ��
		1. ����ض����ṩ�ӿڣ�
			ObjectPool(size_t n = BASE_OBJECT_SIZE, Constructor&& constructor = [] {return new ObjectClass; }):
				parameter:
					n:size_t ������صĳ�ʼ��С,
					constructor:Constructor&& ������ض���洢�Ķ���������(Ĭ��ΪĬ�Ϲ���.)

			inline std::shared_ptr<T> ObjectPool<T>::Get()
				parameter:
					����һ������.

		2. ����ش洢��class�������clean��Ա����������̳�Class Clean.

		3. ʹ��#define AUTO_EXTEND��������ص��Զ����ǹ��ܣ���������ʱ�Զ���������. Ĭ�Ϲرմ˹�����.(ps: �Զ�������鲻������.)

	@note:
		������ڴ����ʱ��ֻ�����ڴ����

	&also:
		����������IOCģ�ͣ�����չ�������Ͷ�������<typeId, Object>; �����������ӿڻ��ø��������Ȳ�������.
*/


/*

	//���object = buffer
	!!!����ؿ���ͨ��ʹ��mutiset�������ȷ�����buffer. ���������Ļ�bufferʼ�����ڲ��ϵĳ���ģ�ֱ���ﵽ�ٽ�ֵ������ʱ���·��䣬��ô��ʲô�취�������أ�

*/



#include<list>
#include<memory>
#include<atomic>
#include<mutex>
#include<functional>
#include<stdexcept>
#include<string>
#include<map>
#include<condition_variable>
#include<type_traits>





namespace gdl {

#define MAX_OBJECT_SIZE	2000
#define BASE_OBJECT_SIZE 10



#define HAS_MEMBER(member)\
template <class T, class... Args>struct has_member##member\
{\
private:\
	template<class U>static auto Check(int) -> decltype(std::declval<U>().member(std::declval<Args>()...), std::true_type());\
	template<class U>static std::false_type Check(...);\
public:\
	enum{value = std::is_same<decltype(Check<T>(0)), std::true_type>::value};\
};\


	HAS_MEMBER(clean)

		/*
		class Counter {
		using Trigger = std::function<void()>;
		public:
			Counter(int n) : count(0), number(n) {};
			void setNumber(int n) {
				if (n > MAX_OBJECT_SIZE)
					throw std::invalid_argument("invalid arguments. ");
				number = n;
			}
			void trigger(int c, Trigger&& cb) {

			}
		private:
			int count;
			int number;
		};
	*/



	class Clean {

	public:
		Clean() = default;
		virtual~Clean() = default;
		virtual void clean() = 0;
	};







	template<class T>
	class ObjectPool {

		using Constructor = std::function<T* (void)>;
	public:
		template<class ObjectClass = T>
		ObjectPool(size_t n = BASE_OBJECT_SIZE, Constructor && constructor = [] {return new ObjectClass; }) : _cap(n), creator(constructor)
		{
			if (n > MAX_OBJECT_SIZE)
				throw std::invalid_argument("invalid arguments, n must less than " + std::to_string(MAX_OBJECT_SIZE));
			this->alloc(n);
			exited = false;
		}



		inline std::shared_ptr<T> Get()
		{
			std::unique_lock<std::mutex> lock(mut);
#ifdef AUTO_EXTEND
			extend();
#endif
			cv.wait(lock, [this] { return size() > 0; });
			std::shared_ptr<T> shptr = pool.front();
			pool.pop_front();
			return shptr;
		}



		template<class M = T>
		inline typename std::enable_if<!has_memberclean<M>::value>::type alloc(size_t n)
		{
			throw std::runtime_error("ObjectPool<T>, class T must has function member: T::clean.");
		}

		template<class M = T>
		inline typename std::enable_if<has_memberclean<M>::value>::type alloc(size_t n)
		{
			for (size_t i = 0; i < n; i++) {
				pool.emplace_back(std::shared_ptr<T>(creator(), [this](T* ptr) {
					deletor(ptr);
					}));
			}
		}
		~ObjectPool() { exited = true; };
		size_t size() { return pool.size(); }
		size_t capcity() { return _cap; }
	private:

		inline void extend()
		{
			if (0 == size()) {
				if (MAX_OBJECT_SIZE > capcity()) {
					size_t n = MAX_OBJECT_SIZE - capcity();
					n = n > BASE_OBJECT_SIZE ? BASE_OBJECT_SIZE : n;
					alloc(n);
					_cap += n;
					cv.notify_all();
				}
			}
		}

		void deletor(T* ptr) {
			if (exited) {
				delete ptr;
				return;
			}
			//T���������л�����������, �ӿ�. �Ӷ�ʹ�����ڻ��պ���Ը���.
			ptr->clean();
			std::unique_lock<std::mutex> lock(mut);
			pool.emplace_back(std::shared_ptr<T>(ptr, [this](T* p) {
				deletor(p);
				}));
			lock.unlock();
			cv.notify_one();
		}


	private:
		bool exited;
		size_t _cap;
		std::mutex mut;
		std::condition_variable cv;
		Constructor creator;
		std::list<std::shared_ptr<T> > pool;
	};




}



#endif