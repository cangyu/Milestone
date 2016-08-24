#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cstdlib>
#include <algorithm>
#include <functional>

namespace sjtu 
{
//a container like std::list
template<typename T>
class list
{
private:
	class node
	{
		friend class list<T>;

	public:
		node *prev, *next;
		T data;

		node(const T &_data) :data(_data)
		{
			prev = next = this;
		}

		node(const node &rhs) :data(rhs.data)
		{
			prev = next = this;
		}

		node& operator=(node rhs)
		{
			exchange(rhs);
			return *this;
		}

		~node() = default;

	private:
		void exchange(node &rhs)
		{
			std::swap(data, rhs.data);
		}

		//given node a and b, insert a before b
		static void insert_before(node *a, node *b)
		{
			b->prev->next = a;
			a->next = b;
			a->prev = b->prev;
			b->prev = a;
		}

		//given node a and b, insert b after a
		static void insert_after(node *a, node *b)
		{
			b->next = a->next;
			a->next = b;
			b->next->prev = b;
			b->prev = a;
		}

		//link node a and b in the form of a<->b
		static void link(node *a, node *b)
		{
			a->next = b;
			b->prev = a;
		}

		//extract single node from list
		static void extract_single(node *t)
		{
			t->prev->next = t->next;
			t->next->prev = t->prev;
			t->prev = t->next = t;
		}
	};

	node *last;
	size_t elemCnt;

	void exchange(list &rhs)
	{
		std::swap(last, rhs.last);
		std::swap(elemCnt, rhs.elemCnt);
	}

public:
	//iterator that knows the ascription
	class const_iterator;
	class iterator 
	{			
		friend class const_iterator;
		friend class list<T>;

	private:
		list<T> *ascription;
		node *cur;

	public:
		iterator(list<T> *_a = nullptr, node *_c = nullptr) : 
			ascription(_a), 
			cur(_c)
		{}

		iterator(const iterator &rhs) :
			ascription(rhs.ascription), 
			cur(rhs.cur)
		{}

		~iterator() = default;
		
		//iter++
		iterator operator++(int) 
		{
			iterator tmp(*this);
			++*this;
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
			--*this;
			return tmp;
		}

		//--iter
		iterator& operator--() 
		{
			cur = cur->prev;
			return *this;
		}

		//*iter
		T& operator*() const 
		{ 
			return cur->data;
		}
		
		//iter->
		T* operator->() const 
		{ 
			return &(operator*()); 
		}

		//check whether two iterators are same (pointing to the same memory).
		bool operator==(const iterator &rhs) const 
		{ 
			if (!ascription || !rhs.ascription || ascription != rhs.ascription)
				return false;
			else if (!cur || !rhs.cur || cur != rhs.cur)
				return false;
			else
				return true;
		}
		
		bool operator==(const const_iterator &rhs) const 
		{ 
			if (!ascription || !rhs.ascription || ascription != rhs.ascription)
				return false;
			else if (!cur || !rhs.cur || cur != rhs.cur)
				return false;
			else
				return true;
		}
		
		bool operator!=(const iterator &rhs) const 
		{ 
			return !operator==(rhs); 
		}
		
		bool operator!=(const const_iterator &rhs) const 
		{ 
			return !operator==(rhs);
		}

		//self check
		bool isValid(void *id) const
		{
			if (!id || (void *)ascription != id || !ascription->last)
				return false;

			node *p = ascription->last;
			do {
				if (cur == p)
					return true;
				else
					p = p->next;
			} while (p != ascription->last);

			return false;
		}
	};

	//Same as iterator, but can't modify data through it
	class const_iterator 
	{
		friend class iterator;
		friend class list<T>;

	private:
		const list<T> *ascription;
		const node *cur;

	public:
		//default constructor
		const_iterator(const list<T> *_a = nullptr, const node *_c = nullptr) :
			ascription(_a),
			cur(_c)
		{}

		//construct from iterator
		const_iterator(const iterator &rhs) :
			ascription(rhs.ascription),
			cur(rhs.cur)
		{}

		//copy-constructor
		const_iterator(const const_iterator &rhs) :
			ascription(rhs.ascription),
			cur(rhs.cur)
		{}

		//destructor
		~const_iterator() = default;

		//iter++
		const_iterator operator++(int)
		{
			iterator tmp(*this);
			++*this;
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
			--*this;
			return tmp;
		}

		//--iter
		const_iterator& operator--()
		{
			cur = cur->prev;
			return *this;
		}

		//*iter
		const T& operator*() const
		{
			return cur->data;
		}

		//iter->
		const T* operator->() const
		{
			return &(operator*());
		}

		//check whether two iterators are same (pointing to the same memory).
		bool operator==(const iterator &rhs) const
		{
			if (!ascription || !rhs.ascription || ascription != rhs.ascription)
				return false;
			else if (!cur || !rhs.cur || cur != rhs.cur)
				return false;
			else
				return true;
		}

		bool operator==(const const_iterator &rhs) const
		{
			if (!ascription || !rhs.ascription || ascription != rhs.ascription)
				return false;
			else if (!cur || !rhs.cur || cur != rhs.cur)
				return false;
			else
				return true;
		}

		bool operator!=(const iterator &rhs) const
		{
			return !operator==(rhs);
		}

		bool operator!=(const const_iterator &rhs) const
		{
			return !operator==(rhs);
		}

		//self check
		bool isValid(void *id) const
		{
			if (!id || (void *)ascription != id || !ascription->last)
				return false;

			node *p = ascription->last;
			do {
				if (cur == p)
					return true;
				else
					p = p->next;
			} while (p != ascription->last);

			return false;
		}
	};
	
	//Constructor
	list() :
		elemCnt(0),
		last((node *)std::malloc(sizeof(node)))
	{
		last->prev = last->next = last;
	}

	list(const list &rhs) :
		elemCnt(rhs.elemCnt),
		last((node *)std::malloc(sizeof(node)))
	{
		last->prev = last->next = last;

		node *p = nullptr;
		node *t = rhs.last->next;

		while (t != rhs.last)
		{
			p = new node(*t);
			node::insert_before(p, last);
			t = t->next;
		}
	}

	//Destructor
	~list()
	{
		clear();
		free(last);
	}

	//assignment operator
	list &operator=(list rhs) 
	{
		exchange(rhs);
		return *this;
	}

	//access the first element
	//throw container_is_empty when it is empty
	const T& front() const 
	{
		if (size() == 0)
			throw container_is_empty();
		
		return last->next->data;
	}

	//access the last element
	//throw container_is_empty when it is empty
	const T& back() const 
	{
		if (size() == 0)
			throw container_is_empty();

		return last->prev->data;
	}

	//returns an iterator to the beginning
	iterator begin()
	{ 
		return iterator(this, last->next);
	}

	const_iterator cbegin() const
	{ 
		return const_iterator(this, last->next);
	}

	//returns an iterator to the end
	iterator end()
	{ 
		return iterator(this, last);
	}

	const_iterator cend() const 
	{ 
		return const_iterator(this, last); 
	}

	//checks whether the container is empty
	bool empty() const 
	{ 
		return size() == 0; 
	}

	//returns the number of elements
	size_t size() const 
	{ 
		return elemCnt; 
	}
	
	//clears the contents
	void clear() 
	{ 
		node *p = last->next;
		node *t = nullptr;

		while (p != last)
		{
			t = p;
			p = p->next;
			delete t;
		}

		elemCnt = 0;
		last->prev = last->next = last;
	}

	//inserts value before pos
	//returns an iterator pointing to the inserted value
	iterator insert(iterator pos, const T &value) 
	{
		if (pos.ascription != this)
			throw invalid_iterator();

		node *p = new node(value);
		node::insert_before(p, pos.cur);
		++elemCnt;

		return iterator(this, p);
	}

	//removes the element at pos.
	//returns an iterator following the last removed element.
	//If there was anything wrong with the iterator, throw invalid_iterator.
	//If the iterator pos refers to the last element, the end() iterator is returned.
	iterator erase(iterator pos) 
	{
		if (pos.ascription != this || pos.cur == last)
			throw invalid_iterator();

		node::link(pos.cur->prev, pos.cur->next);
		node *p = pos.cur->next;
		delete pos.cur;
		--elemCnt;

		return iterator(this, p);
	}

	//removes the elements in range [first, last).
	//returns an iterator following the last removed element.
	//If there was anything wrong with these iterators, throw invalid_iterator.
	//In this case, return last.
	iterator erase(iterator _first, iterator _last) 
	{
		//check ascription
		if (_first.ascription != this || _last.ascription != this || !_first.cur || !_last.cur)
			throw invalid_iterator();

		//check range
		node *start = _first.cur, *finish = _last.cur, *t = nullptr;
		for (t = start; t != finish; t = t->next)
			if (!t || t == last)
				throw invalid_iterator();
		
		//splice
		node::link(start->prev, finish);
		
		//remove
		size_t eraseCnt = 0;
		while (start != finish)
		{
			t = start;
			start = start->next;
			delete t;
			++eraseCnt;
		}
		elemCnt -= eraseCnt;

		return iterator(this, finish);
	}

	//adds an element to the end
	void push_back(const T &value) 
	{ 
		insert(end(), value); 
	}
	
	//removes the last element
	void pop_back() 
	{ 
		erase(--end());
	}

	//adds an element to the beginning
	void push_front(const T &value)
	{ 
		insert(begin(), value);
	}
	
	//removes the first element
	void pop_front() 
	{ 
		erase(begin()); 
	}

	//Sort the elements in [beg,end) in target using given compare object.
	//For better understanding the idea behind, refer to: https://www.zhihu.com/question/31478115/answer/52104149
	template<class Compare=std::less<T>>
	static void sort(list<T> &target, const iterator &beg, const iterator &end, Compare cmp)
	{
		//Pre-Check
		if (beg.ascription != &target)	throw invalid_iterator("beg","Not belongs to target!\n");
		if (end.ascription != &target)	throw invalid_iterator("end", "Not belongs to target!\n");

		int cnt = 0;
		for (iterator t = beg; t != end; ++t)
		{
			if (t.cur == target.last) 
				throw invalid_iterator("beg", "Invalid begining position!\n");
			++cnt;
		}
		if (cnt <= 1)	return;
		
		//Merge Sort
		list<T> counter[64];
		node *r = beg.cur->prev;
		int srcIndex = 0;

		while (beg != end)
		{
			auto t = beg.cur;
			++beg;

			node::extract_single(t);
			insert_before(t, counter[0].last);
			++counter[0].elemCnt;

			while (!counter[srcIndex + 1].empty())//merge
			{
				//merge counter[srcIndex] and counter[srcIndex+1] to counter[srcIndex+1]
				auto e1 = counter[srcIndex].last;
				auto e2 = counter[srcIndex + 1].last;
				auto p1 = e1->next;
				auto p2 = e2->next;
				auto tmp = p1;

				while (p1 != e1 && p2 != e2)
				{
					if (comp(*p1, *p2))
					{
						tmp = p1;
						p1 = p1->next;

						--counter[srcIndex].elemCnt;
						++counter[srcIndex + 1].elemCnt;
					}
					else
					{
						tmp = p2;
						p2 = p2->next;
					}

					node::extract_single(tmp);
					insert_after(e2, tmp);
				}

				while (p1 != e1)
				{
					tmp = p1;
					p1 = p1->next;

					--counter[srcIndex].elemCnt;
					++counter[srcIndex + 1].elemCnt;

					node::extract_single(tmp);
					insert_after(e2, tmp);
				}

				while (p2 != e2)
				{
					tmp = p2;
					p2 = p2->next;

					node::extract_single(tmp);
					insert_after(e2, tmp);
				}

				++srcIndex;//next round
			}

			//move counter[srcIndex] to counter[srcIndex+1]
			counter[srcIndex].exchange(counter[srcIndex + 1]);
			++srcIndex;//本次归并后元素所在的最终counter
		}

		//re-link results to beg and end
		auto p = counter[srcIndex].last;

		r->next = p->next;
		p->prev->next = end.cur;
		end.cur->prev = p->prev;
		p->prev->prev = r;

		p->prev = p->next = p;
		counter[srcIndex].elemCnt = 0;
	}
};

}

#endif
