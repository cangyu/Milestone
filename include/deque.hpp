#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstddef>
#include <cstring>
#include <cmath>

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

		node() :
			start(nullptr), 
			left(nullptr), 
			totalLen(0),
			validLen(0)
		{
			prev = next = this;
		}

		node(size_t _len) :
			start(::operator new T[_len]),
			totalLen(_len),
			validLen(0)
		{
			prev = next = this;
			left = start;
		}

		~node()
		{
			delete[] start;
			start = left = nullptr;
			totalLen = validLen = 0;
		}

		node(const node &rhs) :
			totalLen(rhs.validLen), 
			validLen(rhs.validLen)
		{
			prev = next = this;

			start = ::operator new T[validLen];
			for (int i = 0; i < validLen; i++)
				new (start + i) T(*(rhs.left + i));

			left = start;
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
			std::swap(prev, rhs.prev);
			std::swap(next, rhs.next);
			std::swap(start, rhs.start);
			std::swap(left, rhs.left);
			std::swap(totalLen, rhs.totalLen);
			std::swap(validLen, rhs.validLen);
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
		std::swap(hasSplited, rhs.hasSplited);
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

		// return the distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const 
		{
			if (this->origin != rhs.origin)
				throw invalid_iterator();

			return rhs.getIndex() - this->getIndex();
		}
		iterator& operator+=(const int &n)
		{
			if (n > 0)
			{
				int curRemainCnt = ascription->validLen - (cur - ascription->left) - 1;
				if (n <= curRemainCnt)
					cur += n;
				else
				{
					n -= curRemainCnt;
					node *p = ascription->next;
					while (n >= p->validLen)
					{
						n -= p->validLen;
						p = p->next;
					}

					cur = p->left + n - 1;
					ascription = p;
				}
			}
			else if (n<0)
			{
				n = -n;
				int frontLeftCnt = cur->ascription->left;
				if (n <= frontLeftCnt)
					cur -= n;
				else
				{
					n -= frontLeftCnt;
					node *p = ascription->prev;
					while (n >= p->validLen)
					{
						n -= p->validLen;
						p = p->prev;
					}

					cur = p->left + p->validLen - 1 - n;
					ascription = p;
				}
			}

			return *this;
		}
		iterator& operator-=(const int &n)
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
			//不支持++end()
			if (cur == nullptr)
				throw invalid_iterator();
			
			if (cur == ascription->left + ascription->validLen - 1)
			{
				node *p = ascription->next;
				while (p != origin && p->validLen == 0)
					p = p->next;

				cur = p->left;
				ascription = p;
			}
			else
				++cur;

			return *this;
		}
		iterator operator--(int)
		{
			iterator tmp = *this;
			--*this;
			return tmp;
		}
		iterator& operator--()
		{
			//不支持--begin()
			if (ascription == origin->next&& cur == ascription->left)
				throw invalid_iterator();

			if (cur == ascription->left)//指向当前node的头部
			{
				node *p = ascription->prev;
				while (p != origin && p->validLen == 0)
					p = p->prev;

				if (p == origin)//--end()
					cur = p->left;
				else
					cur = p->left + p->validLen - 1;
				
				ascription = p;
			}
			else
				--cur;

			return *this;
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
			return !operator==(rhs);
		}
		bool operator!=(const const_iterator &rhs) const 
		{
			return !operator==(rhs);
		}

		//check if the iterator is valid:
		bool isValid(void *id) const
		{
			if (!ascription || !origin)//属性标识不合法
				return false;

			if ((void*)origin!= id)//不属于由id指示的deque，类型转换优先级高于双目
				return false;
			
			if (ascription == origin)//end()
				return cur == nullptr;
			else
			{
				node *p = origin->next;
				while (p != origin && p != ascription)
					p = p->next;

				if (p == origin)//ascription不存在
					return false;
				else
				{
					if (p->left - cur > 0)//cur落在left之前
						return false;
					else if (cur - p->left >= p->validLen)//cur超出了[left,left+validLen)
						return false;
					else
						return true;
				}
			}
		}

	private:
		int getIndex() const
		{
			int cnt = 0;

			node *p = origin->next;
			while (p!=origin && p != ascription)
			{
				cnt += p->validLen;
				p = p->next;
			}

			if (p != origin)
				cnt += (cur - ascription->left);
			
			return cnt;
		}
	};
	class const_iterator
	{
		friend class iterator;

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
		const_iterator() :
			cur(nullptr), 
			ascription(nullptr), 
			origin(nullptr)
		{}
		const_iterator(T *_cur, node *_a, node *_ori) :
			cur(_cur), 
			ascription(_a), 
			origin(_ori)
		{}
		const_iterator(const const_iterator &rhs) :
			cur(rhs.cur), 
			ascription(rhs.ascription), 
			origin(rhs.origin)
		{}
		const_iterator(const iterator &rhs) :
			cur(rhs.cur), 
			ascription(rhs.ascription), 
			origin(rhs.origin)
		{}

		~const_iterator() {}

		const_iterator& operator=(const_iterator rhs)
		{
			exchange(rhs);
			return *this;
		}

		const_iterator operator+(const int &n) const
		{
			const_iterator tmp = *this;
			return tmp += n;
		}
		const_iterator operator-(const int &n) const
		{
			const_iterator tmp = *this;
			return tmp -= n;
		}

		// return the distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const
		{
			if (this->origin != rhs.origin)
				throw invalid_iterator();

			return rhs.getIndex() - this->getIndex();
		}
		
		//move n steps
		const_iterator& operator+=(const int &n)
		{
			if (n > 0)
			{
				int curRemainCnt = ascription->validLen - (cur - ascription->left) - 1;
				if (n <= curRemainCnt)
					cur += n;
				else
				{
					n -= curRemainCnt;
					node *p = ascription->next;
					while (n >= p->validLen)
					{
						n -= p->validLen;
						p = p->next;
					}

					cur = p->left + n - 1;
					ascription = p;
				}
			}
			else if (n<0)
			{
				n = -n;
				int frontLeftCnt = cur->ascription->left;
				if (n <= frontLeftCnt)
					cur -= n;
				else
				{
					n -= frontLeftCnt;
					node *p = ascription->prev;
					while (n >= p->validLen)
					{
						n -= p->validLen;
						p = p->prev;
					}

					cur = p->left + p->validLen - 1 - n;
					ascription = p;
				}
			}

			return *this;
		}
		const_iterator& operator-=(const int &n)
		{
			return *this += -n;
		}

		//single move
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			++*this;
			return tmp;
		}
		const_iterator& operator++()
		{
			//不支持++end()
			if (cur == nullptr)
				throw invalid_iterator();

			if (cur == ascription->left + ascription->validLen - 1)
			{
				node *p = ascription->next;
				while (p != origin && p->validLen == 0)
					p = p->next;

				cur = p->left;
				ascription = p;
			}
			else
				++cur;

			return *this;
		}
		const_iterator operator--(int)
		{
			const_iterator tmp = *this;
			--*this;
			return tmp;
		}
		const_iterator& operator--()
		{
			//不支持--begin()
			if (ascription == origin->next&& cur == ascription->left)
				throw invalid_iterator();

			if (cur == ascription->left)//指向当前node的头部
			{
				node *p = ascription->prev;
				while (p != origin && p->validLen == 0)
					p = p->prev;

				if (p == origin)//--end()
					cur = p->left;
				else
					cur = p->left + p->validLen - 1;

				ascription = p;
			}
			else
				--cur;

			return *this;
		}

		//key character of an iterator
		const T& operator*() const
		{
			return *cur;
		}

		const T* operator->() const noexcept
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
			return !operator==(rhs);
		}
		bool operator!=(const const_iterator &rhs) const
		{
			return !operator==(rhs);
		}

		//check if the iterator is valid:
		bool isValid(void *id) const
		{
			if (!ascription || !origin)//属性标识不合法
				return false;

			if ((void*)origin != id)//不属于由id指示的deque，类型转换优先级高于双目
				return false;

			if (ascription == origin)//end()
				return cur == nullptr;
			else
			{
				node *p = origin->next;
				while (p != origin && p != ascription)
					p = p->next;

				if (p == origin)//ascription不存在
					return false;
				else
				{
					if (p->left - cur > 0)//cur落在left之前
						return false;
					else if (cur - p->left >= p->validLen)//cur超出了[left,left+validLen)
						return false;
					else
						return true;
				}
			}
		}

	private:
		int getIndex() const
		{
			int cnt = 0;

			node *p = origin->next;
			while (p != origin && p != ascription)
			{
				cnt += p->validLen;
				p = p->next;
			}

			if (p != origin)
				cnt += (cur - ascription->left);

			return cnt;
		}

	};

	deque():
		elemCnt(0),
		head(new node()),
		hasSplited(false)
	{}

	deque(const deque &rhs) :
		elemCnt(rhs.elemCnt), 
		head(deepCopy(rhs)), 
		hasSplited(false)
	{
		maintain();
	}

	~deque()
	{
		clear();
		delete head;
		hasSplited = false;
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
		const size_t sn = std::sqrt(elemCnt);

		node *p = head->next, *q = head->next, *r = head;
		size_t curCnt = 0;

		while (p != head)
		{
			//找到本次终结点
			while (q != head && curCnt + q->validLen <= sn)
			{
				curCnt += q->validLen;
				q = q->next;
			}

			//create new node containing all the elements
			node *tmp = new node(curCnt);
			size_t k = 0;
			for (auto t = p; t != q; t = t->next)
			{
				if (t->validLen != 0)
				{
					for (auto i = 0; i < t->validLen;i++)
						new (tmp->left + k++) T(*(t->left+i))
				}
			}
			tmp->validLen = curCnt;

			//删掉中间的零碎节点
			auto t = p;
			while (t != q)
			{
				auto c = t;
				t = t->next;
				delete c;
			}

			//ReLink
			r->next = tmp;
			tmp->next = q;
			q->prev = tmp;
			tmp->prev = r;

			//next round
			r = tmp;
			p = q;
			curCnt = 0;
		}
	}

	//深拷贝一个deque
	node* deepCopy(const deque& rhs)
	{
		node *curHead = new node();

		node *p = rhs.head->next;

		while (p != rhs.head)
		{
			if (p->validLen != 0)
			{
				node *tmp = new node(*p);
				insert_before(curHead, tmp);
			}
			p = p->next;
		}

		return curHead;
	}
};

}

#endif
