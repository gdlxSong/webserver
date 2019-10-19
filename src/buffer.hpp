#ifndef BUFFER_GDL_HPP
#define BUFFER_GDL_HPP

#include<string>
#include<string.h>
#include<stdlib.h>
#include<memory>
#include"objectpool.hpp"


namespace gdl {

#define MAX_BUFFER_SIZE		(10*1024*1024)
#define MAX_BUFFER_EXTEND_SIZE	(64*1024)
#define MIN_EMPTY_SIZE			(4*1024)
#define REALLOC_BUFFER_SIZE		(512 * 1024)


	template<size_t SIZE>
	struct MaxSize {
		constexpr static size_t size = SIZE;
	};

	namespace BufferTool {
		class Buffer : public gdl::Clean {

		public:
			Buffer(size_t extendSize = MaxSize<MAX_BUFFER_EXTEND_SIZE>::size);
			Buffer(Buffer&&);

			virtual ~Buffer();

			char* data();
			char* end();
			size_t size();
			size_t capcity();
			size_t usableSize();
			void eat(size_t size);
			void extend(size_t size = 0);//size == 0 : defaul buffer extend _extendSize;
			void loaded(size_t n);

			//clean and reuse.
			void clean() override;
		private:

			size_t reserved();
			std::unique_ptr<char[]> buf;
			size_t _beg, _end, _cap, _extendSize;
		};



		class Buffer_view {

		public:

		};

	}


}




#endif