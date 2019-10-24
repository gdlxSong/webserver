#include"log.hpp"
#include"address.hpp"
#include"threadpool.hpp"
#include<iostream>
#include<functional>
#include"Connector.hpp"
#include"timer.hpp"


int main() {

	

	//test ThreadPool.
	gdl::ThreadPool thpool(1);

	gdl::ServerConn server("www.yqun.xyz", 8848, [&](std::function<void()>&& task) {
		thpool.commit(std::forward<std::function<void()>&&>(task)); 
		});

	server.serverInit();

	sleep(3);
	std::cout << server.serverDescriptions() << std::endl;

	server.start(100);

	while(true)
		sleep(10000);
	//thpool.join();




	return 0;
}