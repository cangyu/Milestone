#include <iostream>
#include <algorithm>
#include "../../include/algorithm.hpp"
#include <random>
#include <chrono>
#include <ctime>
#include <vector>
#include <functional>
#include "../../include/vector.hpp"

using namespace std;

default_random_engine engine((unsigned)time(NULL));

void TestArraySort()
{
	static int a[1000];
	static int b[1000];
	uniform_int_distribution<int> u(0, 1000000);
	for (int i = 0; i < 1000; ++i) {
		b[i] = a[i] = u(engine);
	}
	std::sort(a, a + 1000);
	sjtu::sort(b, b + 1000);
	for (int i = 0; i < 1000; ++i) {
		if (a[i] != b[i]) {
			throw "sort plain array failed";
		}
	}
	std::sort(a, a + 1000, greater<int>());
	sjtu::sort(b, b + 1000, greater<int>());
	for (int i = 0; i < 1000; ++i) {
		if (a[i] != b[i]) {
			throw "sort plain array failed";
		}
	}
}

void TestStdVectorSort()
{
	const int sizeN = 100000;
	std::vector<int> a;
	std::vector<int> b;
	uniform_int_distribution<int> u(0, 121214545);
	for (int i = 0; i < sizeN; ++i) {
		int k = u(engine);
		a.push_back(k);
		b.push_back(k);
	}
	std::sort(a.begin(), a.end());
	sjtu::sort(b.begin(), b.end());
	for (int i = 0; i < sizeN; ++i) {
		if (a[i] != b[i]) {
			throw "sort std::vector failed";
		}
	}
	std::sort(a.begin(), a.end(), greater<int>());
	sjtu::sort(b.begin(), b.end(), greater<int>());
}

void TestSjtuVector()
{
	const int sizeN = 100000;
	std::vector<int> a;
	sjtu::vector<int> b;
	uniform_int_distribution<int> u(0, 121214545);
	for (int i = 0; i < sizeN; ++i) {
		int k = u(engine);
		a.push_back(k);
		b.push_back(k);
	}
	std::sort(a.begin(), a.end());
	sjtu::sort(b.begin(), b.end());
	for (int i = 0; i < sizeN; ++i) {
		if (a[i] != b[i]) {
			throw "sort std::vector failed";
		}
	}
	std::sort(a.begin(), a.end(), greater<int>());
	sjtu::sort(b.begin(), b.end(), greater<int>());
	for (int i = 0; i < sizeN; ++i) {
		if (a[i] != b[i]) {
			throw "sort std::vector failed";
		}
	}
}

void TestMakeHeap()
{
	static int a[1001] = {0};
	uniform_int_distribution<int> u(0, 1215415);
	for (int i = 1; i <= 1000; ++i) {
		a[i] = u(engine);
	}
	sjtu::make_heap(a + 1, a + 1 + 1000);
	for (int i = 1; i <= 1000; ++i) {
		if ((i << 1) <= 1000) {
			if (a[i] < a[i << 1]) {
				throw "make heap failed";
			}
		}
		if ((i << 1) + 1 <= 1000) {
			if (a[i] < a[i << 1]) {
				throw "make heap failed";
			}
		}
	}
}

int main()
{
	try {
		//TestArraySort();
		//TestStdVectorSort();
		//TestSjtuVector();
		TestMakeHeap();
	} catch (const char *s) {
		cout << s << endl;
		return 0;
	}
	cout << "pass all" << endl;

	system("pause");
	return 0;
}