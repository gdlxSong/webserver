




#ifndef SOCKTOOL_GDL_HPP
#define SOCKTOOL_GDL_HPP

#include <errno.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/epoll.h>

namespace gdl {

	using socket_t = int;
	struct socktool {
		static int setNonBlock(socket_t fd, bool value = true);
		static int setReuseAddr(socket_t fd, bool value = true);
		static int setReusePort(socket_t fd, bool value = true);
		static int setNoDelay(socket_t fd, bool value = true);


		// Wrapper function, for restat while sinal interrupt. __fortify_function ssize_t recv(int __fd, void* __buf, size_t __n, int __flags)
		static ssize_t Wrecv(int __fd, void* __buf, size_t __n, int __flags);
		static ssize_t Wsend(int __fd, const void* __buf, size_t __n, int __flags);


		//extern int epoll_wait(int __epfd, struct epoll_event* __events, int __maxevents, int __timeout);
		static int Wepoll_wait(int __epfd, struct epoll_event* __events, int __maxevents, int __timeout);

	};







}






#endif