//provided by Chen Xinhao and Songyu Ke
#include "class-integer.hpp"
#include "class-matrix.hpp"
#include "class-bint.hpp"

#include "list.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <list>

const int N = 107;
using namespace std;
template <typename T> 
class TypeA : public sjtu::list<T> 
{
	int GG;
};

long long randNum(long long x,long long maxNum)
{
	x = (x * 10007) % maxNum;
	return x + 1; 
} 
void TestInteger()
{
	std::cout << "Test for classes without default constructor..." << std::endl;
	sjtu::list<Integer> vInt;
	for (int i = 1; i <= N; ++i) {
		vInt.push_back(Integer(randNum(i, N + 17)));
	}
	int k = 1;
	for(sjtu::list<Integer> :: iterator i = vInt.begin(); ++k <= N; ++k)
		if(randNum(k, 17) <= 15)
			i = vInt.erase(i);
		else 
			++i;
	vInt.erase(vInt.begin(), vInt.end());
	cout << vInt.empty() << endl;
	const sjtu::list<Integer> cInt = vInt; 
	std::cout << "Test OK..." << std::endl;
}

void TestMatrix()
{
	std::cout << "Test for my Matrix..." << std::endl;
	sjtu::list<Diamond::Matrix<double>> vM;
	for (int i = 1; i <= N; ++i) {
		vM.push_back(Diamond::Matrix<double>(randNum(i + 1, 10 + 7), randNum(i + 2, 10 + 7), randNum(i + 3, (100 + 17)) * 1.0 / randNum(i, 17)));
	}
	vM.push_front(vM.back());
	vM.push_back(vM.front()); 
	int k = 1;
	for(sjtu::list<Diamond::Matrix<double>> :: iterator i = vM.begin(); ++k <= N; )
		if(randNum(k, 17) <= 16)
			i = vM.erase(i);
		else
			++i;
	cout << "Size :" << vM.size() << endl;
	for(sjtu::list<Diamond::Matrix<double>> :: iterator i = vM.begin(); i != vM.end(); ++i)
	{
		std::cout << *i << std::endl;
	}
	sjtu::list<Diamond::Matrix<double>> :: iterator First = vM.begin(), Last = vM.end();
	++First, ++First, --Last, --Last;
	vM.erase(First, Last);
	cout << "Size :" << vM.size() << endl;
	for(sjtu::list<Diamond::Matrix<double>> :: iterator i = vM.begin(); i != vM.end(); ++i)
	{
		std::cout << *i << std::endl;
	}
	const sjtu::list<Diamond::Matrix<double>> cM = vM;
	cout << "Size :" << cM.size() << endl;
	for(sjtu::list<Diamond::Matrix<double>> :: const_iterator i = cM.cbegin(); i != cM.cend(); ++i)
	{
		std::cout << *i << std::endl;
	}
	
	while(!vM.empty()) vM.pop_back();
	
}

void TestBint()
{
	std::cout << "Test for big integer" << std::endl;
	sjtu::list<Util::Bint> vBint;
	for (long long i = 1LL << 50; i < (1LL << 50) + N; ++i) {
		vBint.push_back(Util::Bint(i) * randNum(i, (1 << 25) ));
	}
	vBint.push_front(vBint.back());
	vBint.push_back(vBint.front()); 
	int k = 1;
	for(sjtu::list<Util::Bint> :: iterator i = vBint.begin(); ++k <= N; ++k)
		if(randNum(k, 29) <= 25)
			i = vBint.erase(i);
		else
			++i;
	cout << "Size :" << vBint.size() << endl;
	for(sjtu::list<Util::Bint> :: iterator i = vBint.begin(); i != vBint.end(); ++i)
	{
		std::cout << *i << endl;
	}
	sjtu::list<Util::Bint> :: iterator First = vBint.begin(), Last = vBint.end();
	++First, ++First, ++First, --Last, --Last, --Last;
	vBint.erase(First, Last);
	cout << "Size :" << vBint.size() << endl;
	for(sjtu::list<Util::Bint> :: iterator i = vBint.begin(); i != vBint.end(); ++i)
	{
		std::cout << *i << std::endl;
	}
	const sjtu::list<Util::Bint> cBint = vBint;
	cout << "Size :" << cBint.size() << endl;
	for(sjtu::list<Util::Bint> :: const_iterator i = cBint.cbegin(); i != cBint.cend(); ++i)
	{
		std::cout << *i << std::endl;
	}
	while(!vBint.empty()) vBint.pop_back();
}

template <typename T>
void TestDFS(int deep, const int maxDeep, T tem)
{
	sjtu::list<T> l;
	for(int i = 1; i <= 3; ++i)
		l.push_front(tem);
	l.push_front(l.back());
	l.push_back(l.front());
	const sjtu::list<T> cl = l;
	l = l = l = l = l = cl;
	l = l = l = l = l = l;
	if(deep < maxDeep) TestDFS<T>(deep + 1, maxDeep, tem);
	for(int i = 1; i <= 3; ++i)
		l.pop_front();
	l.erase(l.begin());
	l.erase(--l.end());
}

void TestErase()
{
	sjtu::list<int> a, b;
	for (int i = 1; i < 11; ++i) {
		a.push_back(i);
		b.push_back(i);
	}
	for (int i = 1; i < 6; ++i) {
		a.pop_front();
	}
	try {
		//throw 1;
		sjtu::list<int>::iterator it = a.begin();
		++it;
		b.erase(it);
	} catch (...) {
		std::cout << "Throw correctly" << std::endl;
	}
	try {
		//throw 1;
		b.erase(a.begin(), a.end());
	} catch (...) {
		std::cout << "Throw correctly" << std::endl;
	}
	try {
		//throw 1;
		a.erase(a.end(), a.begin());
	} catch (...) {
		std::cout << "Throw correctly" << std::endl;
	}
}

int main()
{
	freopen("./TestingResults/list/list_basic.out", "w", stdout);

	TestInteger();
	TestMatrix();
	TestBint();
	TestDFS<Integer>(0, 100, Integer(randNum(N, N + 17)));
	TestDFS<Diamond::Matrix<double>>(0, 100, Diamond::Matrix<double>(randNum(7, 10 + 7), randNum(7, 10 + 7), randNum(7 + 3, (100 + 17)) * 1.0 / randNum(7, 17)));
	TestDFS<Util::Bint>(0, 100, Util::Bint(randNum(N, N + 17)) * 1000000000ll * 1000000000ll * 1000000000ll);
	TestDFS<TypeA<Integer>>(0, 100, TypeA<Integer>());
	TestDFS<TypeA<Diamond::Matrix<double>>>(0, 100, TypeA<Diamond::Matrix<double>>());
	TestDFS<TypeA<Util::Bint>>(0, 100, TypeA<Util::Bint>());
	TestErase();
	std :: cout << "Okay" << endl;
	return 0;
}
