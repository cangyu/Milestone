#include "../../include/list.hpp"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int N=100000000;

int main()
{
	vector<int> va(N,0);
	sjtu::list<int> vl;

	for(int i=N;i>0;i--)
	{
		va[N-i]=i;
		vl.push_back(i);
	}

	sort(va.begin(),va.end());
	sjtu::list<int>::sort(vl,vl.begin(),vl.end());

	bool result = true;

	auto it=vl.begin();
	for (int i = 0; i < N; i++)
	{
		if (va[i] != *it++)
		{
			result = false;
			break;
		}
	}

	cout << (result ? "Pass!" : "Fail!")<< endl;
	return 0;
}
