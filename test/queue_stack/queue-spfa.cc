#include <iostream>
#include <random>
#include <list>
#include <vector>
#include <queue>

#include "../../include/list.hpp"
#include "../../include/queue.hpp"

std::default_random_engine myRandom(1021233);

const int n = 100;
const int m = 1000;

const int limitN = n + 5;

struct Edge {
	int v, w;
	constexpr Edge() : v(0), w(0) {}
	Edge(const int &_v, const int &_w) : v(_v), w(_w) {}
	Edge(const Edge &other) : v(other.v), w(other.w) {}
};

sjtu::list<Edge> to[limitN];
sjtu::queue<int> que;
bool inQueue[limitN] = {0};
int dist[limitN] = {0};

void ConstructGraph()
{
	std::uniform_int_distribution<int> vertex(1, n);
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

void Spfa()
{
	std::fill(dist, dist + n + 1, ~0U >> 3);
	dist[1] = 0;
	inQueue[1] = true;
	que.push(1);
	while (!que.empty()) {
		int u = que.front();
		inQueue[u] = false;
		que.pop();
		for (sjtu::list<Edge>::iterator it = to[u].begin(); it != to[u].end(); ++it) {
			if (dist[u] + it->w < dist[it->v]) {
				dist[it->v] = dist[u] + it->w;
				if (!inQueue[it->v]) {
					que.push(it->v);
					inQueue[it->v] = true;
				}
			}
		}
	}
}

int main()
{
	ConstructGraph();
	Spfa();
	for (int i = 1; i <= n; ++i) {
		std::cout << "dist[" << i << "]=" << dist[i] << std::endl;
	}
	return 0;
}
