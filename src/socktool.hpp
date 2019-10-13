




#ifndef SOCKTOOL_GDL_HPP
#define SOCKTOOL_GDL_HPP

#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>


namespace gdl {

	using socket_t = int;
	struct socktool {
		static int setNonBlock(socket_t fd, bool value = true);
		static int setReuseAddr(socket_t fd, bool value = true);
		static int setReusePort(socket_t fd, bool value = true);
		static int setNoDelay(socket_t fd, bool value = true);
	};







}






#endif