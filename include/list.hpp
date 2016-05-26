#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cstdlib>
#include <algorithm>

namespace sjtu 
{

template<typename T>
class list
{
public:
	//The elementry component of a list
	typedef struct node
	{
		node *prev, *next;
		T *data;

		//Constructors
		node(node *_prev = nullptr, node *_next = nullptr, T *_data = nullptr) :prev(_prev), next(_next), data(_data) {}
		node(node* _prev, node *_next, const T &_data) :prev(_prev), next(_next)//可否直接new？
		{
			data = (T*)malloc(sizeof(T));
			new (data) T(_data);
		}

		//Destructor
		~node()
		{
			prev = next = nullptr;
			if (!data)
			{
				data->~T();
				free(data);
			}
			data = nullptr;
		}

	}NodeTypeDef, *pNodeTypeDef;

private:
	pNodeTypeDef last;
	size_t elemCnt;

	void swap(list &rhs)
	{
		using std::swap;
		swap(last, rhs.last);
		swap(elemCnt, rhs.elemCnt);
	}

public:
	//Iterator that knows the bound
	class const_iterator;
	class iterator 
	{			
	private:
		pNodeTypeDef cur;
		pNodeTypeDef bound;

		void swap(iterator &rhs)
		{
			using std::swap;
			swap(cur, rhs.cur);
			swap(bound, rhs.bound);
		}

	public:
		//Constructors
		iterator(pNodeTypeDef _cur = nullptr, pNodeTypeDef _bound = nullptr) : cur(_cur), bound(_bound) {}
		iterator(const iterator &rhs) :cur(rhs.cur), bound(rhs.bound) {}
		
		//assignment operator,using the copy-on-swap strategy
		iterator& operator=(iterator rhs)
		{
			this->swap(rhs);
			return *this;
		}
		
		//Self-Check
		bool isValid() const
		{
			pNodeTypeDef tmp = cur->next;
			while (tmp && tmp != cur)
			{
				if (tmp == bound)
					return true;
				tmp = tmp->next;
			}
			return false;
		}

		//Check if this iterator belongs to certain list
		bool checkBelonging(pNodeTypeDef out) const { return bound == out; }

		//iter++
		iterator operator++(int) 
		{
			iterator tmp(*this);
			cur = cur->next;
			return tmp;
		}

		 //++iter
		iterator& operator++() 
		{
			cur = cur->next;
			return *this;
		}

		//iter--
		iterator operator--(int) 
		{
			iterator tmp(*this);
			cur = cur->prev;
			return tmp;
		}

		//--iter
		iterator& operator--() 
		{
			cur = cur->prev;
			return *this;
		}

		//*iter
		T& operator*() const { return *(cur->data); }
		
		//iter->
		T* operator->() const { return cur->data; }

		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
		bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
		
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
		bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }

		//link two iterator to avoid repetiton
		static void link(iterator &lhs, iterator &rhs)
		{
			lhs.cur->next = rhs.cur;
			rhs.cur->prev = lhs.cur;
		}

		//release the data memory
		void release() { cur->~node(); }
	};

	//Same as iterator,but can't modify data through it
	class const_iterator 
	{
	private:
		pNodeTypeDef cur;
		pNodeTypeDef bound;

		void swap(const_iterator &rhs)
		{
			using std::swap;
			swap(cur, rhs.cur);
			swap(bound, rhs.bound);
		}

	public:
		//Constructors
		const_iterator(pNodeTypeDef _cur, pNodeTypeDef _bound) :cur(_cur), bound(_bound) {}
		const_iterator(const const_iterator &rhs) :cur(rhs.cur), bound(rhs.bound) {}
		
		const_iterator& operator=(const_iterator rhs)
		{
			this->swap(rhs);
			return *this;
		}

		//Self-Check
		bool isValid() const
		{
			pNodeTypeDef tmp = cur->next;
			while (tmp && tmp != cur)
			{
				if (tmp == bound)
					return true;
				tmp = tmp->next;
			}
			return false;
		}

		//Check if this const_iterator belongs to certain list
		bool checkBelonging(pNodeTypeDef out) const { return bound == out; }

		//iter++
		const_iterator operator++(int)
		{
			const_iterator tmp(*this);
			cur = cur->next;
			return tmp;
		}

		//++iter
		const_iterator& operator++()
		{
			cur = cur->next;
			return *this;
		}

		//iter--
		const_iterator operator--(int)
		{
			const_iterator tmp(*this);
			cur = cur->prev;
			return tmp;
		}

		//--iter
		const_iterator& operator--()
		{
			cur = cur->prev;
			return *this;
		}

		//*iter
		T operator*() const { return *(cur->data); }

		//iter->
		T* operator->() const { return cur->data; }

		/**
		* a operator to check whether two iterators are same (pointing to the same memory).
		*/
		bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
		bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
		/**
		* some other operator for iterator.
		*/
		bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
		bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }

		//link two iterator to avoid repetiton
		static void link(const_iterator &lhs, const_iterator &rhs)
		{
			lhs.cur->next = rhs.cur;
			rhs.cur->prev = lhs.cur;
		}

		//release the data memory
		void release() { cur->~node(); }
	};
	
	//Constructors
	list() :elemCnt(0)
	{
		last = (pNodeTypeDef)malloc(sizeof(NodeTypeDef));
		new (last) NodeTypeDef(last, last, nullptr);
	}
	list(const list &other)
	{
		last = (pNodeTypeDef)malloc(sizeof(NodeTypeDef));
		new (last) NodeTypeDef(last, last, nullptr);
		elemCnt = 0;//在随后的push_back操作中会自动加上

		iterator tmp(other.begin()), tail(other.end());
		while (tmp != tail)
		{
			push_back(*tmp);
			++tmp;
		}
	}

	//Destructor
	~list()
	{
		clear();
		free(last);
	}

	/**
	 * assignment operator
	 */
	list &operator=(list rhs) 
	{
		this->swap(rhs);
		return *this;
	}

	/**
	 * access the first element
	 * throw container_is_empty when it is empty.
	 */
	const T & front() const 
	{
		if (size() == 0)
			throw container_is_empty();
		
		return *(last->next->data);
	}

	/**
	 * access the last element
	 * throw container_is_empty when it is empty.
	 */
	const T & back() const 
	{
		if (size() == 0)
			throw container_is_empty();

		return *(last->prev->data);
	}

	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() const { return iterator(last->next, last); }
	const_iterator cbegin() const { return const_iterator(last->next, last); }

	/**
	 * returns an iterator to the end.
	 */
	iterator end() const { return iterator(last, last); }
	const_iterator cend() const { return const_iterator(last, last); }

	/**
	 * checks whether the container is empty.
	 */
	bool empty() const { return elemCnt == 0; }

	/**
	 * returns the number of elements.
	 */
	size_t size() const { return elemCnt; }
	
	/**
	 * clears the contents.
	 */
	void clear() { erase(begin(), end()); }

	/**
	 * inserts value before pos.
	 * returns an iterator pointing to the insert value.
	 */
	iterator insert(iterator pos, const T &value) 
	{
		if (!pos.checkBelonging(last))
			throw invalid_iterator();

		pNodeTypeDef dataNode = new NodeTypeDef(nullptr, nullptr, value);
		iterator curNode(dataNode, last), prev(pos);
		--prev;
		iterator::link(prev, curNode);
		iterator::link(curNode, pos);
		++elemCnt;
		return curNode;
	}

	/**
	 * removes the element at pos.
	 * returns an iterator following the last removed element.
	 * If there was anything wrong with the iterator, throw invalid_iterator.
	 * If the iterator pos refers to the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) 
	{
		if (pos == end() || !pos.checkBelonging(last))//attention should be paid to check if the given iterator is within this list!
			throw invalid_iterator();

		iterator prev(pos), next(pos);
		--prev;
		++next;
		iterator::link(prev, next);
		pos.release();
		--elemCnt;
		return next;
	}

	/**
	 * removes the elements in range [first, last).
	 * returns an iterator following the last removed element.
	 * If there was anything wrong with these iterators, throw invalid_iterator.
	 * In this case, return last.
	 */
	iterator erase(iterator first, iterator last) 
	{
		if (!rangeCheck(first, last))//what's the rule for judgement? not across?
			throw invalid_iterator();

		iterator prev(first), cur(first);
		--prev;
		size_t num = 0;
		while (cur != last)
		{
			iterator tmp(cur++);
			tmp.release();
			++num;
		}
		elemCnt -= num;
		iterator::link(prev, last);
		return last;
	}

	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) { insert(end(), value); }
	
	/**
	 * removes the last element.
	 */
	void pop_back() { erase(--end()); }

	/**
	 * adds an element to the beginning
	 */
	void push_front(const T &value) { insert(begin(), value); }
	
	/**
	 * removes the first element.
	 */
	void pop_front() { erase(begin()); }

	bool rangeCheck(const iterator &lhs, const iterator &rhs) const
	{
		if (!lhs.checkBelonging(last) || !rhs.checkBelonging(last))
			return false;

		iterator tmp(lhs), tail(end());
		while (tmp != rhs)//要求区间不横跨last
		{
			if (tmp == tail)
				return false;
			++tmp;
		}

		return true;
	}
};

}

#endif