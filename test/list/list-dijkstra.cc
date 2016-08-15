// Provided by Xinhao Chen
#include <iostream>
#include <random>
#include <list>
#include <vector>
#include <queue>
#include <utility>
#include <cstring>

#include "../../include/list.hpp"

std::default_random_engine myRandom(32421);

const int n = 1000 + 5;
const int m = 50000 + 5;
typedef std::pair<double,int> Pair;

struct Edge {
	int v, w;
	constexpr Edge() : v(0), w(0) {}
	Edge(const int &_v, const int &_w) : v(_v), w(_w) {}
	Edge(const Edge &other) : v(other.v), w(other.w) {}
};

sjtu::list<Edge> to[n];
bool flag[n] = {0};
int d[n] = {0};

void ConstructGraph()
{
	std::uniform_int_distribution<int> vertex(1, 100);
	std::uniform_int_distribution<int> weight(5, 10000);
	for (int i = 0; i < m; ++i) {
		int u = vertex(myRandom);
		int v;
		do {
			v = vertex(myRandom);
		} while (u == v);
		int w = weight(myRandom);
		to[u].push_back(Edge(v, w));
	}
}

void Dijkstra(int start)
{
	std::fill(d, d + n, ~0U >> 3);
	memset(flag,0,sizeof(flag));
	d[start] = 0;
	std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> q;
	q.push(std::make_pair(d[start], start));
	while(!q.empty())
	{
		Pair top = q.top();
		q.pop();
		int x = top.second;
		if(flag[x]) continue;
		for(std::list<Edge>::iterator it = to[x].begin(); it != to[x].end(); ++it)
		{
			if(it -> w + d[x] < d[it -> v])
			{
				d[it -> v] = d[x] + it -> w;
				q.push(std::make_pair(d[it -> v], it -> v));
			}	
		}
	}
}

int main()
{
	ConstructGraph();
	std::cout << "Shortest Path from Node 1" << std::endl;
	Dijkstra(1);
	for (int i = 1; i <= 100; ++i) {
		std::cout << "dist[" << i << "]=" << d[i] << std::endl;
	}
	std::cout << "Shortest Path from Node 10" << std::endl;
	Dijkstra(10);
	for (int i = 1; i <= 100; ++i) {
		std::cout << "dist[" << i << "]=" << d[i] << std::endl;
	}
	std::cout << "Shortest Path from Node 100" << std::endl;
	Dijkstra(100);
	for (int i = 1; i <= 100; ++i) {
		std::cout << "dist[" << i << "]=" << d[i] << std::endl;
	}
	return 0;
}
