#ifndef GDL_TIMER_H
#define GDL_TIMER_H

#include<chrono>
#include <sys/time.h>
#include<string>





namespace gdl {

	//��Ҫע����ǣ�GCC�Ͱ汾����v4.x.x��֧��std::put_time�� ������һ�£��ҵ�gcc7.3.1��֧�ֵ�.

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

}



#endif