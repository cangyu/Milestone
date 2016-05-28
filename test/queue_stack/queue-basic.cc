#include <iostream>
#include <queue>
#include "queue.hpp"
#include "../common/class-bint.hpp"

sjtu::queue<int> queInt;
sjtu::queue<Util::Bint> queBint;

int main()
{
	freopen("queue_basic.out", "w", stdout);

	for (int i = 1; i <= 100; ++i) {
		queInt.push(i);
		queBint.push(Util::Bint(i));
	}
	sjtu::queue<Util::Bint> duplicateQueBint(queBint);
	sjtu::queue<int> duplicateQueInt;
	duplicateQueInt = queInt;
	std::cout << "a queue for int" << std::endl; 
	while (!queInt.empty()) {
		std::cout << queInt.front() << " ";
		queInt.pop();
	}
	std::cout << std::endl;
	std::cout << "a queue for bint" << std::endl;
	while (!queBint.empty()) {
		std::cout << queBint.front() << " ";
		queBint.pop();
	}
	std::cout << std::endl;
	std::cout << "a queue for int" << std::endl; 
	while (!duplicateQueInt.empty()) {
		std::cout << duplicateQueInt.front() << " ";
		duplicateQueInt.pop();
	}
	std::cout << std::endl;
	std::cout << "a queue for bint" << std::endl;
	while (!duplicateQueBint.empty()) {
		std::cout << duplicateQueBint.front() << " ";
		duplicateQueBint.pop();
	}
	std::cout << std::endl;
}