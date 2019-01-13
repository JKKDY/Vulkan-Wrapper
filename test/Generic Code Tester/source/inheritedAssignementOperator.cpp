#pragma once
#include <iostream>
#define PRINT(MSG) std::cout<<##MSG##<<std::endl;


class Base {
public : 
	Base() {
		std::cout << "Constructor base" << std::endl;
	}
	~Base() {
		std::cout << "Destructor base" << std::endl;
	}
	Base& operator=(const Base& a) {
		std::cout << "Assignment base" << std::endl;
		return *this;
	}
};



class Derived {
public:
	Derived() {
	std::cout << "Constructor Derived" << std::endl;
	}
	~Derived() {
	 std::cout << "Destructor Derived" << std::endl;
	}
	//Derived& operator=(const Derived& a) {
	// std::cout << "Assignment Derived" << std::endl;
	//}
};


inline void test1() {
	Base b;
	Base a;
	a = b;

	std::cout << "ekjbfbhkdsbfk" << std::endl;

	Derived c;
	Derived d;
	c = d;
}
