#include "../../include/vector.hpp"
#include <iostream>
#include <vector>
#include <ctime>

int main()
{
	clock_t start, finish;
	start = clock();

	sjtu::vector<long long> v;
	for (long long i = 0; i < 1LL << 20; ++i) {
		v.push_back(i);
	}
	std::cout << v.back() << std::endl;
	for (long long i = 0; i < 1LL << 11; ++i) {
		v.insert(v.begin(), i);
	}
	for (size_t i = 0; i < 1LL << 10; ++i) {
		std::cout << v.front() << std::endl;
		v.erase(v.begin());
	}

	finish = clock();
	std::cout << (double)(finish - start)/CLOCKS_PER_SEC*1000 << "ms" << std::endl;

	return 0;
}
