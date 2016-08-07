#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cstring>

namespace sjtu
{
template<class T>
class deque
{
private:
	class node
	{
	public:
		node *prev, *next;
		T *start, *left;
        size_t totalLen, validLen;

		node() :start(nullptr), end(nullptr), len(0)
		{
			prev = next = this;
		}

		node(size_t _len) :start(new T[_len]), len(0)
		{
			prev = next = this;
			end = start + _len;
		}

		~node()
		{
			delete[] start;
			start = end = nullptr;
			len = 0;
		}

		node(const node &rhs):
			prev(rhs.prev),
			next(rhs.next)
		{
			len = rhs.len;
			start = (T*)::operator new(len * sizeof(T));
			for (int i = 0; i < len; i++)
				new (start + i) T(*(rhs.start + i));
		}

		node &operator=(node rhs)
		{
			exchange(rhs);
			return *this;
		}

	private:
		void exchange(node &rhs)
		{
			std::swap(prev, rhs.prev);
			std::swap(next, rhs.next);
			std::swap(start, rhs.start);
			std::swap(end, rhs.end);
			std::swap(len, rhs.len);
		}
	};

	size_t elemCnt;
	node *head;

	void exchange(deque &rhs)
	{
		std::swap(elemCnt, rhs.elemCnt);
		std::swap(head, rhs.head);
	}

public:
	class const_iterator;
	class iterator
	{
		friend class const_iterator;
	private:
		T *cur;
		node *ascription, *origin;

		void exchange(iterator &rhs)
		{
			std::swap(cur, rhs.cur);
			std::swap(ascription, rhs.ascription);
			std::swap(origin, rhs.origin);
		}

	public:
		iterator() :cur(nullptr), ascription(nullptr), origin(nullptr) {}
		iterator(T *_cur, node *_a, node *_ori) :cur(_cur), ascription(_a), origin(_ori) {}
		iterator(const iterator &rhs) :cur(rhs.cur), ascription(rhs.ascription), origin(rhs.origin) {}
		~iterator() {}
		iterator& operator=(iterator rhs)
		{
			exchange(rhs);
			return *this;
		}

		iterator operator+(const int &n) const
		{
			iterator tmp = *this;
			return tmp += n;
		}
		iterator operator-(const int &n) const
		{
			iterator tmp = *this;
			return tmp -= n;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			//TODO
		}
		iterator operator+=(const int &n)
		{
			//TODO
		}
		iterator operator-=(const int &n)
		{
			return *this += -n;
		}

		iterator operator++(int)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}
		iterator& operator++()
		{

		}
		iterator operator--(int)
		{
			iterator tmp = *this;
			--*this;
			return tmp;
		}
		iterator& operator--()
		{

		}

		T& operator*() const { return *cur; }
		T* operator->() const noexcept { return &(operator*()); }

		bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
		bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
		bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
		bool operator!=(const const_iterator &rhs) const { return cur != rhs.cur; }
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
		public:
			const_iterator() {
				// TODO
			}
			const_iterator(const const_iterator &other) {
				// TODO
			}
			const_iterator(const iterator &other) {
				// TODO
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};

	deque():elemCnt(0),head(new node()) {}

	deque(const deque &other)
	{

	}

	~deque()
	{
		clear();
		delete head;
	}

	deque &operator=(deque other)
	{
		exchange(other);
		return *this;
	}

	//access specified element with bounds checking
	T& at(const size_t &pos)
	{
		if (pos >= elemCnt)
			throw index_out_of_bound();

		node *t = head->next;
		while (pos >= t->len)
		{
			pos -= t->len;
			t = t->next;
		}

		return *(t + pos);
	}
	const T &at(const size_t &pos) const
	{
		if (pos >= elemCnt)
			throw index_out_of_bound();

		node *t = head->next;
		while (pos >= t->len)
		{
			pos -= t->len;
			t = t->next;
		}

		return *(t + pos);
	}
	T& operator[](const size_t &pos)
	{
		return at(pos);
	}
	const T& operator[](const size_t &pos) const
	{
		return at(pos);
	}

	//access the first element
	const T & front() const
	{
		if (elemCnt == 0)
			throw container_is_empty();

		return *(head->next->left);
	}

	//access the last element
	const T & back() const
	{
		if (elemCnt == 0)
			throw container_is_empty();

		return *(head->prev->left+head->prev->validLen-1);
	}

	//iterator to the beginning
	iterator begin() { return iterator(head->next->start, head->next, head); }
	const_iterator cbegin() const { return begin(); }

	//iterator to the end
	iterator end() { return iterator(nullptr, head, head); }
	const_iterator cend() const { return end(); }

    //true if the container is empty
	bool empty() const { return elemCnt == 0; }

    //number of elements in the container
	size_t size() const { return elemCnt; }

    //release all the nodes, as well as the contents inside
	void clear() 
    {
        node *p=head->next;
        node *t=nullptr;
        while(p!=head)
        {
            t=p->next;
            delete p;
            p=t;
        }
    }

	//inserts value before pos
	iterator insert(iterator pos, const T &value) 
    {
        if (!pos.isValid(this))
			throw invalid_iterator();
        
        //当前块内有空余
        if(pos.ascription.validLen<pos.ascription.totalLen)
        {
            if(pos.ascription.start<pos.ascription.left)//左边有空余
            {
                size_t numToMove=pos.cur-pos.ascription->left;
                std::memmove(pos.ascription->left-1,pos.ascription->left,numToMove*sizeof(T));
                *pos.cur=value;
            }
            else//右边有空余
            {
                size_t numToMove=pos.ascription->validLen-(pos.cur-pos.ascription->left);
                std::memmove(pos.cur+1,pos.cur,numToMove*sizeof(T));
                *pos.cur=value;
            }
            ++validLen;
            return pos;
        }
        else//当前块已满
        {
            size_t index=iterator::getIndex(pos);
            
            size_t numToMove=pos.ascription->validLen-(pos.cur-pos.ascription->left);
            node *tmp=new node(numToMove);
            for(int i=0;i<numToMove;i++)
                new (tmp->left+i) T(*(pos.cur+i));
            tmp->validLen=numToMove;
            
            *pos.cur=value;
            pos.ascription->validLen-=(numToMove-1);
            
            insert_after(pos.ascription,tmp);
            
            _maintain(pos.ascription->prev);
            return iterator::getIterator(index);
        }
    }
	
	//removes specified element at pos.
	iterator erase(iterator pos)
	{
		if (empty() || !pos.isValid(this))
			throw invalid_iterator();
        
        size_t index=iterator::getIndex(pos);
        --elemCnt;
        
        if(pos.cur==pos.ascription->left+(validLen-1))
            --pos.ascription->validLen;
        else if(pos.cur==pos.ascription->left)
        {
            ++pos.ascription->left;
            --pos.ascription->validLen;
        }
        else
        {
            size_t numToMove=pos.ascription->validLen-1-(pos.cur-pos.ascription->left);
            std::memmove(pos.cur,pos.cur+1,numToMove*sizeof(T));
            --pos.ascription->validLen;
        }
        
        _maintain(pos.ascription->prev);
        return iterator::getIterator(index);
	}

	void push_back(const T &value)
	{
		node *p = head->prev;
		++elemCnt;
		if (p->right < p->end)
		{
			*p->right = value;
			++p->right;
		}
		else
		{
			node *tmp = new node(1);
			*tmp->start = value;
			insert_after(p, tmp);
			_maintain();
		}
	}

	void pop_back()
	{
		if (empty())
			throw container_is_empty();

		--elemCnt;
		--head->prev->right;

		_maintain();
	}

	void push_front(const T &value)
	{
		node *p = head->next;
		++elemCnt;
		if (p != head && p->left > p->start)
		{
			--p->left;
			*p->left = value;
		}
		else
		{
			node *tmp = new node(1);
			*tmp->start = value;
			insert_after(head, tmp);
			_maintain();
		}
	}

	void pop_front()
	{
		if (empty())
			throw container_is_empty();

		--elemCnt;
		++head->next->left;

		_maintain();
	}

private:
	//在a节点后面插入b节点
	static void insert_after(node *a, node *b)
	{
		b->next = a->next;
		a->next = b;
		b->next->prev = b;
		b->prev = a;
	}

	void _maintain()
	{

	}
};

}

#endif
