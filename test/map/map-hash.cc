#include <iostream>
#include <random>
#include <list>
#include <vector>
#include <queue>
#include <map>
#include "map.hpp"

std::default_random_engine myRandom(1021233);
const int MaxL = 5;
const int N = 100000;
std::string randString()
{
	std::uniform_int_distribution<int> c('A', 'G');
	std::uniform_int_distribution<int> length(1, MaxL);
	std::string temS = "";
	int l = length(myRandom);
	for(int i = 0; i < l; ++i)
		temS += c(myRandom);
	
	return temS;
}
int main()  
{
	freopen("map_hash.out_1", "w", stdout);

	sjtu::map<std::string,bool> have;
    sjtu::map<std::string, std::string> trans;
    for(int i = 1; i <= N; ++i)
	{
		std::string english = randString();
		std::string foreign = randString();
		have[foreign] = true;  
        trans[foreign] = english;  
    }
    for(int i = 1; i <= N / 500; ++i)
    {  
		std::string s = randString();
        if(have[s])  
			std::cout << trans[s] << std::endl;
        else  
            printf("no_find\n");
    }  
}  
