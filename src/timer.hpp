#ifndef GDL_TIMER_H
#define GDL_TIMER_H

#include<chrono>
#include <sys/time.h>
#include<string>





namespace gdl {

	//需要注意的是：GCC低版本，如v4.x.x不支持std::put_time， 测试了一下，我的gcc7.3.1是支持的.

	class Timer {
		
	public:
		//construct Timer and initializer.
		Timer() : m_begin(std::chrono::high_resolution_clock::now()) {}
		void reset() {
			m_begin = std::chrono::high_resolution_clock::now();
		}

		//milliseconds.
		template<class Duration = std::chrono::milliseconds>
		int64_t elapsed()const {
			return std::chrono::duration_cast<Duration>(std::chrono::high_resolution_clock::now() - m_begin).count();
		}

		//microseconds.
		int64_t elapsed_miscc()const {
			return elapsed<std::chrono::microseconds>();
		}

		//nanoseconds
		int64_t elapsed_nano()const {
			return elapsed<std::chrono::nanoseconds>();
		}

		//seconds
		int64_t elapsed_seconds()const {
			return elapsed<std::chrono::seconds>();
		}

		//minutes
		int64_t elapsed_minutes()const {
			return elapsed<std::chrono::minutes>();
		}

		//hours
		int64_t elapsed_hour()const {
			return elapsed<std::chrono::hours>();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
	};

	struct Timestamp {
		static std::string now();
		static std::string getTimestamp(const time_t t);
	};







	namespace TimerSeq {
	
		class TimerSqueue {

			//关于定时队列的设计：
			//1. 定是队列内存储的是定时执行函数{interval, function<void()>}
			//2. 队列是有序的，最早被执行的定时函数在队列头，最晚的在队列尾部.
			//3. 被执行过的定时函数按照下一次执行时间插入队列.
			//4. 此类有一个定时执行的函数，函数执行队列内超时的函数. handleTimeout
			//5. 在epoll模型中，我才用将距离最近超时的时间传递给epoll_wait.
			//6. 为了提高效率，采用模糊超时，就是有一个超时范围，如5ms.


		};
	
	
	
	
	}







}



#endif