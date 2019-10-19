







#include"socktool.hpp"

#include<errno.h>


int gdl::socktool::setNonBlock(socket_t fd, bool value)
{
	int mode = fcntl(fd, F_GETFL);
	mode = value ? mode | O_NONBLOCK : mode & ~O_NONBLOCK;
	return fcntl(fd, F_SETFL, mode);
}

int gdl::socktool::setReuseAddr(socket_t fd, bool value)
{
	int flag = value ? 1 : 0;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof flag);
}

int gdl::socktool::setReusePort(socket_t fd, bool value)
{
	int flag = value ? 1 : 0;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof flag);
}

int gdl::socktool::setNoDelay(socket_t fd, bool value)
{
	int flag = value ? 1 : 0;
	return setsockopt(fd, SOL_SOCKET, TCP_NODELAY, &flag, sizeof(flag));
}

ssize_t gdl::socktool::Wrecv(int __fd, void* __buf, size_t __n, int __flags)
{
	ssize_t sret{ 0 };
	while (-1 == (sret = recv(__fd, __buf, __n, __flags)) && EINTR == errno);
	return sret;
}

ssize_t gdl::socktool::Wsend(int __fd, const void* __buf, size_t __n, int __flags)
{
	ssize_t sret{ -1 };
	while (-1 == (sret = send(__fd, __buf, __n, __flags)) && EINTR == errno);
	return sret;
}

int gdl::socktool::Wepoll_wait(int __epfd, epoll_event* __events, int __maxevents, int __timeout)
{
	int sret{ -1 };
	while (-1 == (sret = epoll_wait(__epfd, __events, __maxevents, __timeout)) && EINTR == errno);
	return sret;
}



