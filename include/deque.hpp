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

		node* getBackPtr() const
		{
			return left + validLen - 1;
		}

		bool leftAvailable() const
		{
			return leftAvailableCnt() != 0;
		}

		bool rightAvailable() const
		{
			return rightAvailableCnt() != 0;
		}

	private:
		void exchange(node &rhs)
		{

		}

		//头部可用空间
		size_t leftAvailableCnt() const
		{
			return left - start;
		}

		//尾部可用空间
		size_t rightAvailableCnt() const
		{
			return totalLen - leftAvailableCnt() - validLen;
		}
	};

	size_t elemCnt;
	node *head;
	bool hasSplited;

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

		//key character of an iterator
		T& operator*() const 
		{ 
			return *cur;
		}
		T* operator->() const noexcept 
		{ 
			return &(operator*());
		}

		//check if two iterators pointing to same memory
		bool operator==(const iterator &rhs) const 
		{ 
			return cur == rhs.cur;
		}
		bool operator==(const const_iterator &rhs) const 
		{ 
			return cur == rhs.cur;
		}
		bool operator!=(const iterator &rhs) const 
		{ 
			return cur != rhs.cur;
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			return cur != rhs.cur;
		}

		bool isValid(void *id) const
		{

		}
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

		return *(find(pos));
	}
	const T& at(const size_t &pos) const
	{
		if (pos >= elemCnt)
			throw index_out_of_bound();

		return *(find(pos));
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
	const T& front() const
	{
		if (elemCnt == 0)
			throw container_is_empty();

		return *(begin());
	}

	//access the last element
	const T& back() const
	{
		if (elemCnt == 0)
			throw container_is_empty();

		return *(--end());
	}

	//iterator to the beginning
	iterator begin() 
	{ 
		node *p = head->next;
		while (p != head && p->validLen == 0)
			p = p->next;

		return iterator(p->left, p, head);
	}
	const_iterator cbegin() const 
	{ 
		return begin(); 
	}

	//iterator to the end
	iterator end() 
	{ 
		return iterator(nullptr, head, head); 
	}
	const_iterator cend() const 
	{ 
		return end(); 
	}

    //true if the container is empty
	bool empty() const 
	{ 
		return elemCnt == 0; 
	}

    //number of elements in the container
	size_t size() const 
	{ 
		return elemCnt; 
	}

    //release all the nodes, as well as the contents inside
	void clear()
	{
		node *p = head->next;
		node *t = nullptr;
		while (p != head)
		{
			t = p->next;
			delete p;
			p = t;
		}
	}

	//inserts value before pos
	iterator insert(iterator pos, const T &value)
	{
		if (!pos.isValid(this))
			throw invalid_iterator();

		++elemCnt;

		if (pos == end())
		{
			node *tmp = new node(1);
			new (tmp->left) T(value);
			++tmp->validLen;

			insert_after(head->prev, tmp);

			return iterator(tmp->left, tmp, head);
		}
		else
		{
			if (pos.ascription.validLen < pos.ascription.totalLen)//当前node有空余
			{
				//[left,pos)之间元素个数
				size_t leftMoveCnt = pos.cur - pos.ascription->left;
				//[pos,left+validLen)之间元素个数
				size_t rightMoveCnt = pos.ascription->validLen - leftMoveCnt;

				//左边空余的数量
				size_t leftEmptyCnt = pos.ascription->left - pos.ascription->start;
				//右边空余的数量
				size_t rightEmptyCnt = pos.ascription->totalLen - leftEmptyCnt - pos.ascription->validLen;

				//确定移动方向
				bool moveToLeft = leftEmptyCnt == 0 ? false : (rightMoveCnt == 0 ? true : leftMoveCnt <= rightMoveCnt);

				//move elements
				if (moveToLeft)
				{
					std::memmove(pos.ascription->left - 1, pos.ascription->left, leftMoveCnt * sizeof(T));
					--pos.ascription->left;
					--pos.cur;
				}
				else
					std::memmove(pos.cur + 1, pos.cur, rightMoveCnt * sizeof(T));

				//insert new value
				*pos.cur = value;
				++pos.ascription->validLen;

				//返回指向新插入元素的iterator
				return pos;
			}
			else//当前node已满
			{
				if (pos.cur == pos.ascription->left && pos.ascription->prev->rightAvailable())//插在头部且前一个node的尾部有空余
				{
					//插在前一个node的尾部
					node *p = pos.ascription->prev;
					*(p->left + p->validLen) = value;
					++p->validLen;

					//返回指向前一个node尾部元素的iterator
					return iterator(p->left + p->validLen - 1, p, head);
				}
				else if (pos.cur == pos.ascription->getBackPtr() && pos.ascription->next->leftAvailable())//插在尾部且后一个node的头部有空余
				{
					//插在后一个node的头部
					node *p = pos.ascription->next;
					*(--p->left) = value;
					++p->validLen;

					//返回指向下一个node头部元素的iterator
					return iterator(p->left, p, head);
				}
				else//split
				{
					node *p = pos.ascription;
					hasSplited = true;

					//[left,pos]之间元素个数
					size_t leftMoveCnt = pos.cur - p->left + 1;
					//[pos,left+validLen)之间元素个数
					size_t rightMoveCnt = p->validLen - leftMoveCnt + 1;

					if (leftMoveCnt <= rightMoveCnt)//左半部分元素较少
					{
						//[left,pos]
						node *tmp = new node(leftMoveCnt);

						//拷贝[left,pos)的原有数据
						for (int i = 0; i < leftMoveCnt - 1; i++)
							new (tmp->left + i) T(*(p->left + i));

						//insert value
						tmp->validLen = leftMoveCnt;
						new (tmp->left + tmp->validLen - 1) T(value);

						//修正原node
						p->left = pos.cur;
						p->validLen -= (leftMoveCnt - 1);

						//将新的node链入原node之前
						insert_before(p, tmp);

						//返回指向新插入的value的iterator，即tmp的尾部
						return iterator(tmp->getBackPtr(), tmp, head);
					}
					else//右半部分元素较少，move them
					{
						//[pos,left+validCnt)
						node *tmp = new node(rightMoveCnt);

						//拷贝[pos,left+validCnt)
						for (int i = 0; i < rightMoveCnt; i++)
							new (tmp->left + i) T(*(pos.cur + i));
						tmp->validLen = rightMoveCnt;

						//insert value
						*pos.cur = value;
						p->validLen -= (rightMoveCnt - 1);

						//将新的node链入原node之后
						insert_after(p, tmp);

						//返回指向新插入的value的iterator，即pos自身
						return pos;
					}
				}
			}
		}
	}
	
	//removes specified element at pos
	iterator erase(iterator pos)
	{
		if (empty() || !pos.isValid(this) || pos == end())
			throw invalid_iterator();

		--elemCnt;
		if (pos.cur == pos.ascription->left + (pos.ascription->validLen - 1))//pos指向当前node的最后一个，此时元素个数>=1
		{
			--pos.ascription->validLen;

			//找到下一个不为空的node
			node *p = pos.ascription->next;
			while (p != head && p->validLen == 0)
				p = p->next;

			//返回该node的第一个元素，若deque中只有一个元素，p将回到head，即end()
			return iterator(p->left, p, head);
		}
		else if (pos.cur == pos.ascription->left)//pos指向当前node的第一个，此时元素个数>=2
		{
			++pos.ascription->left;
			--pos.ascription->validLen;

			//由于不会出现validLen==0的情况，直接返回指向下一个pos的iterator
			++pos.cur;
			return pos;
		}
		else//pos指向当前node的非头尾元素，此时元素个数>=3
		{
			//pos之前的元素个数
			size_t numToMove = pos.cur - pos.ascription->left;

			if (2 * numToMove < pos.ascription->validLen)//前半部分元素少于一半
			{
				//[left,pos)之间的元素向后move一个单位
				std::memmove(pos.ascription->left + 1, pos.ascription->left, numToMove * sizeof(T));
				++pos.ascription->left;
				--pos.ascription->validLen;

				//返回指向pos后面的元素的iterator
				++pos.cur;
				return pos;
			}
			else
			{
				//[pos+1,left+validLen)之间的元素向前move一个单位
				numToMove = pos.ascription->validLen - numToMove - 1;
				std::memmove(pos.cur, pos.cur + 1, numToMove * sizeof(T));
				--pos.ascription->validLen;

				//新的元素替代了pos上原来的元素，直接返回
				return pos;
			}
		}
	}

	//头尾操作
	void push_back(const T &value)
	{
		insert(end(),value);
	}
	void pop_back()
	{
		erase(--end());
	}
	void push_front(const T &value)
	{
		insert(begin(), value);
	}
	void pop_front()
	{
		erase(begin());
	}

private:
	//在a节点之前插入b节点
	static void insert_before(node *a, node *b)
	{
		b->next = a;
		a->prev->next = b;
		b->prev = a->prev;
		a->prev = b;
	}
	
	//在a节点之后插入b节点
	static void insert_after(node *a, node *b)
	{
		b->next = a->next;
		a->next = b;
		b->next->prev = b;
		b->prev = a;
	}

	//根据下标返回iterator
	//确保index合法！
	iterator find(size_t index) const
	{
		if (hasSplited)
		{
			maintain();
			hasSplited = false;
		}

		node *p = head->next;
		while (index >= p->validLen)
		{
			index -= p->validLen;
			p = p->next;
		}

		return iterator(p->left + index, p, head);
	}

	//将链表中较短的node合并
	void maintain()
	{

	}
};

}

#endif
