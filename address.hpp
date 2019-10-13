







#ifndef ADDRESS_GDL_HPP
#define ADDRESS_GDL_HPP

#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>


namespace gdl {


	namespace IpAddr {


		class IpAddrv4 {

		public:
			IpAddrv4(const std::string& host, unsigned short port);
			IpAddrv4(sockaddr_in saddr) { address = saddr; }
			IpAddrv4(IpAddrv4* paddr);
			std::string getIp();
			std::string getPort();
			sockaddr_in getaddr() { return address; }
			bool isValid() { return INADDR_NONE != address.sin_addr.s_addr; }

			static std::string host2Ip(const std::string& host);
		private:
			in_addr getHostByName(const std::string& host);

			sockaddr_in address;
		};


	}




}




#endif