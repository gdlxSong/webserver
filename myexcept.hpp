#include"log.hpp"
#include<unistd.h>

template<class T>
T IfElse(bool expr, T pred1, T pred2) {
	return expr ? pred1 : pred2;
}


template<class Callable>
void exitif(bool expr, Callable&& pred) {
	if (expr) {
		pred();
		sleep(2);
		exit(1);	
	}
}


template<class Callable>
void doif(bool expr, Callable&& pred) {
	if (expr)
		pred();
}