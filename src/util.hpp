



#ifndef HELPER_GDL_HPP
#define HELPER_GDL_HPP

#include<string>
#include<memory>
#include<stdio.h>
#include<unistd.h>
#include <stdarg.h>
namespace gdl {




	struct noncopyable {
		noncopyable() = default;
		virtual ~noncopyable() = default;
		noncopyable(const noncopyable&) = delete;
		noncopyable& operator=(const noncopyable&) = delete;
	};


	using string = std::string;

	namespace strTool {

		std::string format(const char* fmt, ...);


	}






}






#endif