#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <algorithm>
#include <cstdlib>
#include <cstddef>
#include <functional>
#include "exceptions.hpp"

#include "vector.hpp"

namespace sjtu 
{
/**
 * a container like std::priority_queue which is a heap internal.
 * it should be based on the vector written by yourself.
 */
template<typename T, class Compare = std::less<T>, typename HeapTypeDef = binary_heap<T, Compare> >
class priority_queue 
{
private:
	HeapTypeDef *heap;

	void swap(priority_queue &other) { std::swap(heap, other.heap); }

public:
	/**
	 * constructors
	 */
	priority_queue():heap(new HeapTypeDef()) {}
	priority_queue(const priority_queue &other) :heap(new HeapTypeDef(*(other.heap))) {}

	/**
	 * deconstructor
	 */
	~priority_queue() { delete heap; }

	/**
	 * Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) 
	{
		priority_queue tmp(other);
		swap(tmp);
		return *this;
	}

	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const 
	{
		if (empty())
			throw container_is_empty();

		return heap->front();
	}

	/**
	 * push new element to the priority queue.
	 */
	void push(const T &e) { heap->push_back(e); }
	
	/**
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() 
	{
		if (empty())
			throw container_is_empty();

		heap->pop_front();
	}

	/**
	 * return the number of the elements.
	 */
	size_t size() const { return heap->size(); }

	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const { return heap->empty(); }
};


template<typename T, class Compare = std::less<T> >
class binary_heap
{
private:
	sjtu::vector<T> c;
	Compare cmp;

	size_t parent(size_t i) const { return (i - 1) / 2; }
	size_t left(size_t i) const { return 2 * i + 1; }
	size_t right(size_t i) const { return 2 * i + 2; }

	void percolateDown(size_t pos)
	{
		const size_t n = c.size();

		size_t child = left(pos);
		while (child < n)
		{
			if (child + 1 < n && cmp(c[child], c[child+1]))
				++child;

			if (cmp(c[child], c[pos]))
				break;
			else
			{
				std::swap(c[child], c[pos]);
				pos = child;
				child = left(pos);
			}
		}
	}

	void percolateUp(size_t pos)
	{
		size_t father = 0;
		while (pos > 0)
		{
			father = parent(pos);
			if (cmp(c[pos], c[father]))
				break;
			else
			{
				std::swap(c[pos], c[father]);
				pos = father;
				father = parent(pos);
			}
		}
	}

public:
	binary_heap() {}
	binary_heap(const Compare &x):cmp(x) {}

	binary_heap(const binary_heap &other) :c(other.c),cmp(other.cmp) {}

	~binary_heap() {}

	bool empty() const { return c.empty(); }

	size_t size() const { return c.size(); }

	const T &front() const
	{
		if (c.empty())
			throw container_is_empty();

		return c.front();
	}

	void push_back(const T &elem)
	{
		size_t pos = c.size();
		c.push_back(elem);
		percolateUp(pos);
	}

	void pop_front()
	{
		std::swap(c[0], c[size() - 1]);	
		c.pop_back();
		percolateDown(0);

	}
};

}

#endif