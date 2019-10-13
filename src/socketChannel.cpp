#include "socketChannel.hpp"
#include<atomic>
#include"myexcept.hpp"
#include<unistd.h>
#include"log.hpp"
#include"util.hpp"


gdl::SocketChannel::SocketChannel(gdl::SslChannel* _sslChannel, gdl::EPoller* _epoller, int _fd, uint32_t _events)
{
	static std::uint64_t ChannelIdInintializer(0);
	sslChannel = _sslChannel;
	epoller = _epoller;
	fd_ = _fd;
	events = _events;
	channelId = ChannelIdInintializer++;
	readcb = writecb = errorcb = [] {};//initialize
	epoller->addChannel(this);
}



gdl::SocketChannel::~SocketChannel()
{
	//在信道死亡的时候自动注销再epoll的监听.
	close();
}

void gdl::SocketChannel::enableRead(bool flag)
{
	events = IfElse(flag, POLLIN | events, ~POLLIN & events);
	epoller->updateChannel(this);
}

void gdl::SocketChannel::enableWrite(bool flag)
{
	events = IfElse(flag, POLLOUT | events, ~POLLOUT & events);
	epoller->updateChannel(this);
}

void gdl::SocketChannel::enableWriteRead(bool flag)
{
	events = IfElse(flag, POLLIN | events, ~POLLIN & events);
	events = IfElse(flag, POLLOUT | events, ~POLLOUT & events);
	epoller->updateChannel(this);
}

bool gdl::SocketChannel::readable()
{
	return POLLIN & events;
}

bool gdl::SocketChannel::writeable()
{
	return POLLOUT & events;
}

void gdl::SocketChannel::handleRead()
{
	lastrotate = time(NULL);
	readcb();
}

void gdl::SocketChannel::handleWrite()
{
	lastrotate = time(NULL);
	writecb();
}


void gdl::SocketChannel::close()
{
	gdl::Glog().mount(gdl::LogLevel::LINFO, gdl::strTool::format("socketChanel %d closed.", channelId));
	::close(fd_);//自动在EPOLL中注销监听.
	epoller->removeChannel(this);
}
