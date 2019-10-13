#include"log.hpp"
#include"address.hpp"
#include"threadpool.hpp"
#include<iostream>
#include<functional>
#include"Connector.hpp"
#include"timer.hpp"


int main() {

	

	//test ThreadPool.
	gdl::threadpool thpool(5);

	gdl::ServerConn server("www.yqun.xyz", 8848);
	server.serverInit();

	sleep(3);
	std::cout << "server start....." << std::endl;

	while(true)
	thpool.commit([&] {
		server.loop(100);
		});

	return 0;
}