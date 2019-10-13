#include"address.hpp"
#include<string.h>
#include<stdexcept>
#include"util.hpp"


gdl::IpAddr::IpAddrv4::IpAddrv4(const std::string& host, unsigned short port)
{
	//bzero.
	bzero(&address, sizeof(address));
	//family
	address.sin_family = AF_INET;
	//host.
	if (host.empty())
		address.sin_addr.s_addr = INADDR_ANY;
	else
		address.sin_addr = getHostByName(host);
	//port.
	address.sin_port = htons(port);
	
	//check sin_addr
	if (INADDR_NONE == address.sin_addr.s_addr)
		throw std::invalid_argument("invalid arguments. -> host.");
}

gdl::IpAddr::IpAddrv4::IpAddrv4(IpAddrv4* paddr)
{
	this->address = paddr->address;
}

std::string gdl::IpAddr::IpAddrv4::getIp()
{
	uint32_t uip = address.sin_addr.s_addr;
	return gdl::strTool::format("%d.%d.%d.%d", (uip >> 0) & 0xff, (uip >> 8) & 0xff, (uip >> 16) & 0xff, (uip >> 24) & 0xff);
}

std::string gdl::IpAddr::IpAddrv4::getPort()
{
	return std::to_string(ntohs(address.sin_port));
}

std::string gdl::IpAddr::IpAddrv4::host2Ip(const std::string& host)
{
	return gdl::IpAddr::IpAddrv4(host, 0).getIp();
}

in_addr gdl::IpAddr::IpAddrv4::getHostByName(const std::string& host)
{
	struct in_addr addr;
	char buf[1024];
	struct hostent hent;
	struct hostent* he = NULL;
	int herrno = 0;
	memset(&hent, 0, sizeof hent);
	int r = gethostbyname_r(host.c_str(), &hent, buf, sizeof buf, &he, &herrno);
	if (r == 0 && he && he->h_addrtype == AF_INET) {
		addr = *reinterpret_cast<struct in_addr*>(he->h_addr);
	}
	else {
		addr.s_addr = INADDR_NONE;
	}
	return addr;
}


