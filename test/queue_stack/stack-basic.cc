#include <iostream>
#include <stack>
#include "../../include/stack.hpp"
#include "../common/class-bint.hpp"

sjtu::stack<int> stackInt;
sjtu::stack<Util::Bint> stackBint;

int main()
{
	for (int i = 1; i <= 100; ++i) {
		stackInt.push(i);
		stackBint.push(Util::Bint(i));
	}
	sjtu::stack<Util::Bint> duplicateStackBint(stackBint);
	sjtu::stack<int> duplicateStackInt;
	duplicateStackInt = stackInt;
	std::cout << "a stack for int" << std::endl; 
	while (!stackInt.empty()) {
		std::cout << stackInt.top() << " ";
		stackInt.pop();
	}
	std::cout << std::endl;
	std::cout << "a stack for bint" << std::endl;
	while (!stackBint.empty()) {
		std::cout << stackBint.top() << " ";
		stackBint.pop();
	}
	std::cout << std::endl;
	std::cout << "a stack for int" << std::endl; 
	while (!duplicateStackInt.empty()) {
		std::cout << duplicateStackInt.top() << " ";
		duplicateStackInt.pop();
	}
	std::cout << std::endl;
	std::cout << "a stack for bint" << std::endl;
	while (!duplicateStackBint.empty()) {
		std::cout << duplicateStackBint.top() << " ";
		duplicateStackBint.pop();
	}
	std::cout << std::endl;
}