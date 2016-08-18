#ifndef SJTU_QUEUE_HPP
#define SJTU_QUEUE_HPP

#include "list.hpp"
#include "exceptions.hpp"

#include <cstdlib>
#include <algorithm>

namespace sjtu 
{
/**
 * a container whose behavior is similar to a queue.
 * It should be based on the list you had written before.
 */
template<typename T>
class queue 
{
private:
	typedef sjtu::list<T> ContainerTypeDef;
	
	ContainerTypeDef *container;

	void swap(queue &rhs)
	{
		using std::swap;
		swap(container, rhs.container);
	}

public:
	/**
	 * constructors
	 */
	queue() 
	{
		container = (ContainerTypeDef*)malloc(sizeof(ContainerTypeDef));
		new (container) ContainerTypeDef();
	}

	queue(const queue<T> &other) 
	{
		container = (ContainerTypeDef*)malloc(sizeof(ContainerTypeDef));
		new (container) ContainerTypeDef(*(other.container));
	}

	/**
	 * deconstructor
	 */
	~queue() 
	{
		container->~ContainerTypeDef();
		free(container);
	}

	/**
	 * Assignment operator
	 */
	queue &operator=(queue other)
	{
		swap(other);
		return *this;
	}
	
	/**
	 * get the first of the queue.
	 * @return a reference of the first element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & front() const 
	{
		if (empty())
			throw container_is_empty();

		return container->front();
	}

	/**
	 * push new element to the queue.
	 */
	void push(const T &e) { container->push_back(e); }

	/**
	 * delete the first element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop()
	{
		if (empty())
			throw container_is_empty();
		
		container->pop_front();
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const 
	{ 
		return container->size(); 
	}

	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const 
	{ 
		return container->empty();
	}
};

}
#endif