#include <iostream>
#include <random>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include "unordered_map.hpp"
#include "class-bint.hpp"


std::default_random_engine randomEngine((unsigned)time(NULL));

std::string RandomBint()
{
	std::uniform_int_distribution<int> randomInt(-1000000000, 1000000000);
	return std::to_string(randomInt(randomEngine));
}

bool Benchmark()
{
	std::unordered_map<std::string, Util::Bint> stdBintHashMap;
	sjtu::unordered_map<std::string, Util::Bint> myBintHashMap;
	for (int t = 0; t < 10000; ++t) {
		std::string integer = RandomBint();
		stdBintHashMap.insert(std::make_pair(integer, Util::Bint(integer)));
		myBintHashMap.insert(sjtu::pair<std::string, Util::Bint>(integer, Util::Bint(integer)));
	}
	for (int t = 0; t < 10000; ++t) {
		std::string integer = RandomBint();
		Util::Bint inStdMap = stdBintHashMap[integer];
		Util::Bint inMyMap = myBintHashMap[integer];
		if (inStdMap != inMyMap) {
			return false;
		}
	}
	stdBintHashMap.clear();
	myBintHashMap.clear();
	for (int t = 0; t < 30000; ++t) {
		std::string integer = RandomBint();
		stdBintHashMap.insert(std::make_pair(integer, Util::Bint(integer)));
		myBintHashMap.insert(sjtu::pair<std::string, Util::Bint>(integer, Util::Bint(integer)));
	}
	return stdBintHashMap.size() == myBintHashMap.size();
}

int main()
{
	std::cout << (Benchmark() ? "Benchmark passed" : "Benchmark failed") << std::endl;
	return 0;
}
