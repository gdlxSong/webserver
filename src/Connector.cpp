#include "Connector.hpp"
#include<iostream>
#include"timer.hpp"
#include<signal.h>


gdl::ServerConn::ServerConn(const std::string& host, unsigned short port, PusherAble&& psr)
{
	//create epoller.
	epoller = std::make_shared<gdl::EPoller>(new gdl::EPoller());
	//serInfo.
	serAddr = std::make_shared<gdl::IpAddr::IpAddrv4>(new gdl::IpAddr::IpAddrv4(host, port)); 
	//logger init.
	logger.init("./log.log", 50);



	paserContainer = std::shared_ptr<gdl::Protocols::PasrserContainer>(new gdl::Protocols::PasrserContainer());
	//协议解析器注册.
	paserContainer->registerConstructor([](gdl::Protocols::Buffer buf) {
		return gdl::Protocols::HttpPaser().analysis(std::move(buf));
		});

	//线程任务投递器.
	pusher = std::move(psr);
}

gdl::ServerConn::~ServerConn()
{
}

void gdl::ServerConn::serverInit()
{

	//ignore SIGPIPE.
	handleSigle(SIGPIPE, [](int sig) {
		signal(sig, SIG_IGN);
		});

	int fd_listen = socket(AF_INET, SOCK_STREAM, 0);
	exitif(-1 == fd_listen, [&] {
		logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("create socket error %d %s", errno, strerror(errno)));
		logger.handleWrite();
		});



	int iret = gdl::socktool::setNoDelay(fd_listen);
	exitif(-1 == iret, [&] {
		logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("listen socket error %d %s", errno, strerror(errno)));
		logger.handleWrite();
		});
	iret = gdl::socktool::setReuseAddr(fd_listen);
	exitif(-1 == iret, [&] {
		logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("listen socket error %d %s", errno, strerror(errno)));
		logger.handleWrite();
		});


	//bind.
	sockaddr_in saddr = serAddr->getaddr();
	saddr.sin_addr.s_addr = INADDR_ANY;
	iret = bind(fd_listen, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(sockaddr));
	exitif(-1 == iret, [&] {
		logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("bind socket error %d %s", errno, strerror(errno)));
		logger.handleWrite();
		});

	//listen.
	iret = listen(fd_listen, 20);
	exitif(-1 == iret, [&] {
		logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("listen socket error %d %s", errno, strerror(errno)));
		logger.handleWrite();
		});


	//set socket.
	iret = gdl::socktool::setNonBlock(fd_listen);
	exitif(-1 == iret, [&] {
		logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("listen socket error %d %s", errno, strerror(errno)));
		logger.handleWrite();
		});



	//create SocketChannel.
	serChannel = std::make_shared<gdl::SocketChannel>(new gdl::SocketChannel(NULL, epoller.get(), fd_listen, POLLIN));
	serChannel->onRead([this] {
		handleAccept();
		});
}

void gdl::ServerConn::handleAccept()
{
	gdl::Timer timer;
	//accept.
	int fd = serChannel->Fd();
	sockaddr_in localAddr, remoteAddr;
	socklen_t slen = sizeof(remoteAddr);
	bzero(&localAddr, slen);
	bzero(&remoteAddr, slen);

	

	while (true) {

		int clientFd = accept4(fd, (sockaddr*)&remoteAddr, &slen, SOCK_CLOEXEC);
		if (-1 != clientFd) {
			//create connInfo.
			//get peer name.
			
			int iret = getsockname(clientFd, (sockaddr*)&localAddr, &slen);
			if (-1 == iret) {
				//get remote faild.
				//bzero.
				bzero(&localAddr, slen);
				logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("getpeername error %d %s", errno, strerror(errno)));
			}

			//set socket.
			iret = gdl::socktool::setNonBlock(clientFd);
			doif(-1 == iret, [&] {
				logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("set socket nonblock error %d %s", errno, strerror(errno)));
				//block socket.
				//close socket.
				close(clientFd);
				});
			iret = gdl::socktool::setNoDelay(clientFd);
			doif(-1 == iret, [&] {
				logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("set socket nodelay error %d %s", errno, strerror(errno)));
				});
			iret = gdl::socktool::setReuseAddr(clientFd);
			doif(-1 == iret, [&] {
				logger.mount(gdl::LogLevel::LFATAL, gdl::strTool::format("set socket  reuseaddr error %d %s", errno, strerror(errno)));
				logger.handleWrite();
				});


			auto laddr = gdl::IpAddr::IpAddrv4(localAddr);
			auto raddr = gdl::IpAddr::IpAddrv4(remoteAddr);
			

			//create SocketChannel.
			connInfo.emplace_back(ConnInfo{ laddr, raddr, std::shared_ptr<gdl::SocketChannel>(new gdl::SocketChannel(NULL, epoller.get(), clientFd, POLLIN)) });
			//set callback.
			auto& channel = connInfo.back().channel;
			logger.mount(gdl::LogLevel::LINFO, gdl::strTool::format("a channel created, from %s:%s to %s:%s, channelId = %ld.", raddr.getIp().c_str(), raddr.getPort().c_str(), laddr.getIp().c_str(), laddr.getPort().c_str(), channel->Id()));
			channel->onRead([&] {
				//当Channel变为可读之后，Channel的回调readcb只是做了一件事儿：将Channel数据的接收和数据的处理操作打包发送给线程池. 以期在epoll_wait的时候防止阻塞.
				pusher([&] {
					std::cout << "=========paser start." << std::endl;
					paserContainer->pasrser(channel);
					std::cout << "=========paser end." << std::endl;
					});
				});
			
		}
		else {
			if (errno == EAGAIN || EWOULDBLOCK == errno)
				return;
			exitif(true, [&] {
				logger.mount(gdl::LogLevel::LINFO, gdl::strTool::format("accept4 error %d %s.", errno, strerror(errno)));
				logger.handleWrite();
				});
		}

	}


}

void gdl::ServerConn::loop(int waitMs)
{
	std::unique_lock<std::timed_mutex> lock(tmut, std::defer_lock);
	if (lock.try_lock()) {
		if(!exited)
			epoller->loop_once(waitMs);
	}

	pusher([&] {loop(100);
	std::cout << "Push a task of loop." << std::endl;
		});
	
}

void gdl::ServerConn::start(int waitMs)
{
	loop(waitMs);
}

std::string gdl::ServerConn::serverDescriptions()
{
	return std::string("gdlServer. This is a lightweight server frame. Developed by C++ language.\nAthor: gdl.\nVersion: v0.0.1\n");
}


