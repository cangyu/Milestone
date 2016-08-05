#include <iostream>
#include <random>
#include <string>
#include <cstring> 
#include <algorithm>
#include <unordered_map>
#include "unordered_map.hpp"
using namespace std;
default_random_engine myRandom(1021233);
const int MaxL = 5;
const int N = 1e3; 
string randString() {
	uniform_int_distribution<int> c('A', 'G');
	uniform_int_distribution<int> length(1, MaxL);
	string temS = "";
	int l = length(myRandom);
	for(int i = 0; i < l; ++i)
		temS += c(myRandom);
	return temS;
}
int randNum(int max_num) {
	uniform_int_distribution<int> num(1, max_num);
	return num(myRandom);	
}
template<class Key, class T>
bool check(sjtu :: unordered_map<Key, T> my_map, std :: unordered_map<Key, T> std_map) {
	if(my_map.size() != std_map.size()) return 0;
	
	Key std_A[2 * N], my_A[2 * N];
	typename :: sjtu :: unordered_map<Key, T> :: iterator my_it;	
	typename :: std :: unordered_map<Key, T> :: iterator std_it;
	
	int my_A_cnt = 0, std_A_cnt = 0;
	
	for(my_it = my_map.begin(); my_it != my_map.end(); ++my_it) my_A[++my_A_cnt] = my_it -> first; 
	for(std_it = std_map.begin(); std_it != std_map.end(); ++std_it) std_A[++std_A_cnt] = std_it -> first;
	
	if(my_A_cnt != std_A_cnt) return 0;
	
	std :: sort(my_A + 1, my_A + 1 + my_A_cnt);
	std :: sort(std_A + 1, std_A + 1 + std_A_cnt);
	
	for(int i = 1; i <= my_A_cnt; ++i) if(my_A[i] != std_A[i]) return 0;
	
	for(int i = 1; i <= my_A_cnt; ++i) if(my_map[my_A[i]] != std_map[std_A[i]]) return 0;
	
	const typename :: sjtu :: unordered_map<Key, T> my_copy_map = my_map;
	const typename :: std :: unordered_map<Key, T> std_copy_map = std_map;
	
	typename :: sjtu :: unordered_map<Key, T> :: const_iterator my_cit;	
	typename :: std :: unordered_map<Key, T> :: const_iterator std_cit;
	
	my_A_cnt = 0, std_A_cnt = 0;
	
	for(my_cit = my_map.begin(); my_cit != my_map.end(); ++my_cit) my_A[++my_A_cnt] = my_cit -> first; 
	for(std_cit = std_map.begin(); std_cit != std_map.end(); ++std_cit) std_A[++std_A_cnt] = std_cit -> first;
	
	if(my_A_cnt != std_A_cnt) return 0;
	
	std :: sort(my_A + 1, my_A + 1 + my_A_cnt);
	std :: sort(std_A + 1, std_A + 1 + std_A_cnt);
	
	for(int i = 1; i <= my_A_cnt; ++i) if(my_A[i] != std_A[i]) return 0;
	
	if(my_map.bucket_count() <= 0) return 0;
	
	if(my_map.load_factor() < 0) return 0;
	
	return 1;
}

bool IntTest() {
	sjtu :: unordered_map<int ,int> v;
	std :: unordered_map<int ,int> std_v;
	
	for(int i = 1; i <= N; ++i) {
		int tem_num = randNum(N);
		v.insert(sjtu :: pair<int, int>(i, tem_num));
		std_v.insert(std :: pair<int, int>(i, tem_num));
	}
	
	if(!check(v, std_v)) return 0;
	
	for(int i = 1; i <= N / 2; ++i) {
		v.erase(v.find(i));
		std_v.erase(std_v.find(i));
	}
	
	if(!check(v, std_v)) return 0;
	
	return 1;
}

bool StringTest() {
	sjtu :: unordered_map<string,bool>have;  
    sjtu :: unordered_map<string,string>trans; 
	
	std :: unordered_map<string,bool>std_have;  
    std :: unordered_map<string,string>std_trans; 
    
    for(int i = 1; i <= N; ++i) {
        string english = randString();
        string foreign = randString();
		have[foreign] = std_have[foreign] = true;
        trans[foreign] = std_trans[foreign] = english;
	}
	
	for(int i = 1; i <= N / 2; ++i) {
		string s = randString(); 
		if(have[s]) have.erase(have.find(s)), trans.erase(trans.find(s));
		if(std_have[s]) std_have.erase(std_have.find(s)), std_trans.erase(std_trans.find(s));	
	}
	
	if(!check(have, std_have)) return 0;
	if(!check(trans, std_trans)) return 0;
	
	have.clear(); std_have.clear(); trans.clear(); std_trans.clear();
	
	if(!check(have, std_have)) return 0;
	if(!check(trans, std_trans)) return 0;
	 
	return 1; 	
}
bool GGDFS(int deep, sjtu :: unordered_map<int, string> my_v, std :: unordered_map<int, string> std_v) {
	if(deep % 7 == 0) {
		int english = randNum(N);
		my_v[english] = std_v[english] = randString();
	}
	if(deep == 5000)
		return check(my_v, std_v);
	else
		return GGDFS(deep + 1, my_v, std_v);
}

bool GGsimidaTest() {
	sjtu :: unordered_map<int, string> my_v;
	std :: unordered_map<int, string> std_v;
	
	my_v.clear(), std_v.clear();
	
	if(!check(my_v, std_v)) return 0;
	
	const sjtu :: unordered_map<int, string> copy_my_v = my_v;
	const std :: unordered_map<int, string> copy_std_v = std_v;
	
	if(!check(copy_my_v, copy_std_v)) return 0;
	
	for(int i = 1; i <= 100; ++i) {
        int english = randNum(N);
        my_v[english] = std_v[english] = randString();
	}
	
	if(!check(my_v, std_v)) return 0;
	
	const sjtu :: unordered_map<int, string> copy_my_v_2 = my_v;
	const std :: unordered_map<int, string> copy_std_v_2 = std_v;
	
	if(!check(copy_my_v_2, copy_std_v_2)) return 0;
	
	return GGDFS(1, my_v, std_v);
}
int main()  
{
	cout << (StringTest() ? "StringTest Pass " : "StringTest Fail") << endl;
	cout << (IntTest() ? "IntTest Pass " : "IntTest Fail") << endl;
	cout << (GGsimidaTest() ? "GGsimidaTest Pass " : "GGsimidaTest Fail") << endl;
}  
