#include <iostream>
#include <random>
#include <list>
#include <vector>
#include <queue>
#include <map>
using namespace std;
default_random_engine myRandom(1021233);
const int MaxL = 5;
const int N = 100000;
string randString()
{
	uniform_int_distribution<int> c('A', 'G');
	uniform_int_distribution<int> length(1, MaxL);
	string temS = "";
	int l = length(myRandom);
	for(int i = 0; i < l; ++i)
		temS += c(myRandom);
	
	return temS;
}
int main()  
{
	std::map<string,bool>have;  
    std::map<string,string>trans;  
    for(int i = 1; i <= N; ++i)
	{
        string english = randString();
        string foreign = randString();
		have[foreign] = true;  
        trans[foreign] = english;  
    }
    for(int i = 1; i <= N / 500; ++i)
    {  
		string s = randString(); 
        if(have[s])  
            cout << trans[s] << endl;
        else  
            printf("no_find\n");
    }  
}  
