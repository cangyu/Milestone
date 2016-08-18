#ifndef SJTU_STACK_HPP
#define SJTU_STACK_HPP

#include "vector.hpp"
#include "list.hpp"
#include "exceptions.hpp"

#include <cstdlib>
#include <algorithm>

namespace sjtu 
{
/**
 * a container whose behavior is similar to a stack.
 * It should be based on the vector and list you had written before.
 * The default ListType is vector, but you should make sure that 
 * the stack you design is also correct when ListType is sjtu::list.
 */
template<typename T, typename ListType = vector<T>>
class stack 
{
private:

	ListType *container;

	void swap(stack &rhs) 
	{ 
		std::swap(container, rhs.container);
	}

public:
	stack() 
	{
		container = (ListType*)malloc(sizeof(ListType));
		new (container) ListType();
	}

	stack(const stack &other) 
	{
		container = (ListType*)malloc(sizeof(ListType));
		new (container) ListType(*(other.container));
	}

	~stack()
	{
		container->~ListType();
		free(container);
	}

	stack &operator=(stack other) 
	{
		swap(other);
		return *this;
	}
	
	/**
	 * get the top of the stack.
	 * @return a reference of the top element.
	 * throw container_is_empty when empty() returns true.
	 */
	const T & top() const 
	{
		if (empty())
			throw container_is_empty();

		return container->back(); 
	}

	/**
	 * push new element to the stack.
	 */
	void push(const T &e) 
	{ 
		container->push_back(e); 
	}
	
	/**
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() 
	{
		if (empty())
			throw container_is_empty();

		container->pop_back(); 
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
