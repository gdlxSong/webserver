

#ifndef CONNECTOR_HPP
#define CONNECTOR_HPP

#include"util.hpp"
#include"address.hpp"
#include"poller.hpp"
#include<utility>
#include<tuple>
#include<list>
#include<memory>
#include"myexcept.hpp"
#include"socktool.hpp"
#include<atomic>
#include<mutex>

namespace gdl {




	class Connector : gdl::noncopyable {

	public:
		Connector() = default;
		virtual ~Connector() {}

		template<class Callable>
		void handleSigle(int signal, Callable&& cb);

	private:
		
	};



	class ServerConn : Connector {

		struct ConnInfo {
			gdl::IpAddr::IpAddrv4 localAddr;
			gdl::IpAddr::IpAddrv4 remoteAddr;
			std::shared_ptr<gdl::SocketChannel> channel;
		};
	public:
		ServerConn(const std::string& host, unsigned short port = 80);
		virtual ~ServerConn();
		
		void serverInit();

		std::shared_ptr<gdl::EPoller> getEpoller() { return epoller; }

		void handleAccept();

		void loop(int waitMs);

	private:
		std::atomic<bool> exited;
		std::shared_ptr<gdl::EPoller> epoller;
		std::shared_ptr <gdl::IpAddr::IpAddrv4> serAddr;
		std::shared_ptr<gdl::SocketChannel> serChannel;
		std::list<ConnInfo> connInfo;
		gdl::Glog logger;
		std::timed_mutex tmut;
	};







	template<class Callable>
	inline void Connector::handleSigle(int signal, Callable&& cb)
	{
		cb(signal);
	}

}










#endif