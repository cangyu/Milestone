#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <cmath>
#include <new>

namespace sjtu
{
//类似于std::deque,但基于块状链表实现
template<class T>
class deque
{
private:
	class node
	{
		friend class deque<T>;

	private:
		node *prev, *next;
		T *start, *left;
		size_t totalLen, validLen;

		void exchange(node &rhs)
		{
			std::swap(prev, rhs.prev);
			std::swap(next, rhs.next);
			std::swap(start, rhs.start);
			std::swap(left, rhs.left);
			std::swap(totalLen, rhs.totalLen);
			std::swap(validLen, rhs.validLen);
		}

	public:
		node(size_t _len = 0) :
			prev(this),
			next(this),
			start(nullptr),
			left(nullptr),
			totalLen(_len),
			validLen(0)
		{
			if (_len > 0)
				left = start = (T *)std::malloc(_len * sizeof(T));//注意：虽然malloc的参数可以为0，但应尽量避免
		}

		~node()
		{
			if (start)//注意：只在start不为nullptr时才能free
			{
				for (auto i = 0; i < validLen; i++)
					(left + i)->~T();

				std::free(start);//需要在后面避免重复释放
			}

			start = left = nullptr;
			totalLen = validLen = 0;
		}

		node(const node &rhs) :
			prev(this),
			next(this),
			start(nullptr),
			left(nullptr),
			totalLen(rhs.validLen),
			validLen(rhs.validLen)
		{
			if (validLen > 0)
			{
				left = start = (T *)std::malloc(validLen * sizeof(T));//只复制有效的部分
				for (auto i = 0; i < validLen; i++)
					new (left + i) T(*(rhs.left + i));
			}
		}

		node& operator=(node rhs)
		{
			exchange(rhs);//只保证数据部分被有效拷贝，不拷贝指针
			return *this;
		}

		//可用空间的最后一个位置的指针
		T* getBackPtr() const
		{
			return left ? left + validLen - 1 : nullptr;
		}

		//在b之前插入a
		static void insert(node *a, node *b)
		{
			a->next = b;
			b->prev->next = a;
			a->prev = b->prev;
			b->prev = a;
		}

		//在a之后插入b
		static void insert_after(node *a, node *b)
		{
			b->next = a->next;
			a->next->prev = b;
			a->next = b;
			b->prev = a;
		}

		static void link(node *a, node *b)
		{
			a->next = b;
			b->prev = a;
		}
	};

	size_t elemCnt;
	node *last;
	bool *needMaintain;

	void exchange(deque &rhs)
	{
		std::swap(elemCnt, rhs.elemCnt);
		std::swap(last, rhs.last);
		std::swap(needMaintain, rhs.needMaintain);
	}

public:
	class const_iterator;
	class iterator
	{
		friend class const_iterator;
		friend class deque<T>;

	private:
		deque<T> *ascription;
		node *origin;
		T *cur;

		void exchange(iterator &rhs)
		{
			std::swap(ascription, rhs.ascription);
			std::swap(origin, rhs.origin);
			std::swap(cur, rhs.cur);
		}

	public:
		//constructor
		iterator(deque<T> *_a = nullptr, node *_ori = nullptr, T *_cur = nullptr) :
			ascription(_a),
			origin(_ori),
			cur(_cur)
		{}

		iterator(const iterator &rhs) :
			ascription(rhs.ascription),
			origin(rhs.origin),
			cur(rhs.cur)
		{}

		//destructor
		~iterator() = default;

		//assignment
		iterator& operator=(iterator rhs)
		{
			exchange(rhs);
			return *this;
		}

		// return the distance between two iterator,
		// if these two iterators point to different deque, throw invaild_iterator.
		int operator-(const iterator &rhs) const
		{
			if (ascription != rhs.ascription)
				throw invalid_iterator();

			return getIndex() - rhs.getIndex();
		}

		//迭代器前后移动
		iterator operator+(int n) const
		{
			iterator tmp(*this);
			return tmp += n;
		}

		iterator operator-(int n) const
		{
			iterator tmp(*this);
			return tmp -= n;
		}

		iterator& operator+=(int n)
		{
			if (n == 0)
				return *this;
			else if (n < 0)
				return operator-=(-n);
			else
			{
				//Semantics:向后移动n个有效step，若在移动过程中遇到end()则非法！
				while (origin != ascription->last)
				{
					//本node内剩余的可从当前位置前进的步数
					//while中的条件保证了origin->left!=nullptr
					//在迭代过程中，虽然有可能cur落在validLen=0的node上，使得curRemainCnt=-1
					//但是这只会落到下面的else中，并由于+1的存在，不会错误地改变n
					int curRemainCnt = origin->validLen - (cur - origin->left) - 1;

					if (n <= curRemainCnt)//可以在本node内部解决
					{
						cur += n;
						return *this;
					}
					else//移动到下一个node的第一个
					{
						n -= (curRemainCnt + 1);
						origin = origin->next;
						cur = origin->left;
					}
				}

				if (n != 0)//若遇到end()且不为终点，则invalid
					throw invalid_iterator();
				else
					return *this;
			}
		}

		iterator& operator-=(int n)
		{
			if (n == 0)
				return *this;
			else if (n < 0)
				return operator+=(-n);
			else
			{
				//Semantics: 若在移动一步之后遇到end(),则invalid，即不能跨越begin()
				do {
					//前方剩余的可前进的步数，即[left,cur)之间的元素个数
					//注意：nullptr只能用于比较，不能用于运算
					int frontLeftCnt = cur ? cur - origin->left : 0;

					if (n <= frontLeftCnt)
					{
						cur -= n;
						return *this;
					}
					else
					{
						n -= frontLeftCnt;
						origin = origin->prev;
						cur = origin->left ? origin->left + origin->validLen : nullptr;
					}
				} while (origin != ascription->last);

				throw invalid_iterator();//运行到此处必然是跨越了begin
			}
		}

		iterator operator++(int)
		{
			iterator tmp(*this);
			++*this;
			return tmp;
		}

		iterator& operator++()
		{
			operator+=(1);
			return *this;
		}

		iterator operator--(int)
		{
			iterator tmp(*this);
			--*this;
			return tmp;
		}

		iterator& operator--()
		{
			operator-=(1);
			return *this;
		}

		T& operator*() const
		{
			if (!cur)//防止通过end()来赋值
				throw invalid_iterator();

			return *cur;
		}

		T* operator->() const noexcept
		{
			return &(operator*());
		}

		//check if two iterators pointing to same memory
		bool operator==(const iterator &rhs) const
		{
			if (!ascription || ascription != rhs.ascription)
				return false;
			else if (!origin || origin != rhs.origin)
				return false;
			else
				return cur == rhs.cur;//允许指向end()，此时cur为nullptr
		}

		bool operator==(const const_iterator &rhs) const
		{
			if (!ascription || ascription != rhs.ascription)
				return false;
			else if (!origin || origin != rhs.origin)
				return false;
			else
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

		//check if the iterator is valid
		bool isValid(void *id) const
		{
			//检查ascription是否一致
			if (!id || id != ascription) 
				return false;
			
			//OK, they now point to the same deque.
			//检测origin是否存在
			node *terminal = ascription->last;
			node *p = terminal;
			bool exist = false;

			do {
				if (p == origin)
				{
					exist = true;
					break;
				}
				p = p->next;
			} while (p != terminal);
			if (!exist)
				return false;

			//OK, the node that origin pointing to exists.
			//检测cur范围是否valid
			if (cur == nullptr)
				return origin->left == nullptr;//可以是end()
			else if (origin->left == nullptr)
				return false;
			else
			{
				int dist = cur - origin->left;
				return dist >= 0 && dist < origin->validLen;
			}
		}

	private:
		int getIndex() const
		{
			int cnt = 0;
			node *p = ascription->last->next;
			
			while (p != origin)
			{
				cnt += p->validLen;
				p = p->next;
			}

			if (p == ascription->last)
				return cnt;
			else
				return cnt + (cur - p->left);
		}
	};

	class const_iterator
	{
		friend class iterator;
		friend class deque<T>;

	private:
		const deque<T> *ascription;
		const node *origin;
		const T *cur;

		void exchange(const_iterator &rhs)
		{
			std::swap(ascription, rhs.ascription);
			std::swap(origin, rhs.origin);
			std::swap(cur, rhs.cur);
		}

	public:
		//constructor
		const_iterator(const deque<T> *_a = nullptr, const node *_ori = nullptr, const T *_cur = nullptr) :
			ascription(_a),
			origin(_ori),
			cur(_cur)
		{}

		const_iterator(const iterator &rhs) :
			ascription(rhs.ascription),
			origin(rhs.origin),
			cur(rhs.cur)
		{}

		const_iterator(const const_iterator &rhs) :
			ascription(rhs.ascription),
			origin(rhs.origin),
			cur(rhs.cur)
		{}

		//destructor
		~const_iterator() = default;

		//assignment
		const_iterator& operator=(const_iterator rhs)
		{
			exchange(rhs);
			return *this;
		}

		// return the distance between two iterator,
		// if these two iterators point to different deque, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const
		{
			if (ascription != rhs.ascription)
				throw invalid_iterator();

			return getIndex() - rhs.getIndex();
		}

		//迭代器前后移动
		const_iterator operator+(int n) const
		{
			const_iterator tmp(*this);
			return tmp += n;
		}

		const_iterator operator-(int n) const
		{
			const_iterator tmp(*this);
			return tmp -= n;
		}

		const_iterator& operator+=(int n)
		{
			if (n == 0)
				return *this;
			else if (n < 0)
				return operator-=(-n);
			else
			{
				//Semantics:向后移动n个有效step，若在移动过程中遇到end(),则throw invalid_iterator()
				while (origin != ascription->last)
				{
					//本node内剩余的可从当前位置前进的步数,while 条件保证了origin->left!=nullptr
					int curRemainCnt = origin->validLen - (cur - origin->left) - 1;

					if (n <= curRemainCnt)//可以在本node内部解决
					{
						cur += n;
						return *this;
					}
					else//移动到下一个node的第一个
					{
						n -= (curRemainCnt + 1);
						origin = origin->next;
						cur = origin->left;
					}
				}

				//若遇到end()且不为终点，则invalid
				if (n != 0)
					throw invalid_iterator();
				else
					return *this;
			}
		}

		const_iterator& operator-=(int n)
		{
			if (n == 0)
				return *this;
			else if (n < 0)
				return operator+=(-n);
			else
			{
				//Semantics: 若在移动一步之后遇到end(),则invalid，即不能跨越begin()
				do
				{
					//前方剩余的可前进的步数
					//注意：nullptr只能用于比较，不能用于运算
					int frontLeftCnt = cur ? cur - origin->left : 0;

					if (n <= frontLeftCnt)
					{
						cur -= n;
						return *this;
					}
					else
					{
						n -= frontLeftCnt;
						origin = origin->prev;
						cur = origin->left ? origin->left + origin->validLen : nullptr;
					}

				} while (origin != ascription->last);

				//运行到此处必然是跨越了begin
				throw invalid_iterator();
			}
		}

		const_iterator operator++(int)
		{
			const_iterator tmp(*this);
			++*this;
			return tmp;
		}

		const_iterator& operator++()
		{
			operator+=(1);
			return *this;
		}

		const_iterator operator--(int)
		{
			const_iterator tmp(*this);
			--*this;
			return tmp;
		}

		const_iterator& operator--()
		{
			operator-=(1);
			return *this;
		}

		//key utilities of an iterator
		const T& operator*() const
		{
			if (!cur)
				throw invalid_iterator();

			return *cur;
		}

		const T* operator->() const noexcept
		{
			return &(operator*());
		}

		//check if two iterators pointing to same memory
		bool operator==(const iterator &rhs) const
		{
			if (!ascription || ascription != rhs.ascription)
				return false;
			else if (!origin || origin != rhs.origin)
				return false;
			else
				return cur == rhs.cur;//允许指向end()，此时cur为nullptr
		}

		bool operator==(const const_iterator &rhs) const
		{
			if (!ascription || ascription != rhs.ascription)
				return false;
			else if (!origin || origin != rhs.origin)
				return false;
			else
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

		//check if the iterator is valid
		bool isValid(void *id) const
		{
			//检查ascription是否一致
			if (!id || id != ascription)
				return false;

			//OK, they now point to the same deque.
			//检测origin是否存在
			node *terminal = ascription->last;
			node *p = terminal;
			bool exist = false;

			do {
				if (p == origin)
				{
					exist = true;
					break;
				}
				p = p->next;
			} while (p != terminal);
			if (!exist)
				return false;

			//OK, the node that origin pointing to exists.
			//检测cur范围是否valid
			if (cur == nullptr)
				return origin->left == nullptr;//可以是end()
			else if (origin->left == nullptr)
				return false;
			else
			{
				int dist = cur - origin->left;
				return dist >= 0 && dist < origin->validLen;
			}
		}

	private:
		int getIndex() const
		{
			int cnt = 0;
			node *p = ascription->last->next;

			while (p != origin)
			{
				cnt += p->validLen;
				p = p->next;
			}

			if (p == ascription->last)
				return cnt;
			else
				return cnt + (cur - p->left);
		}
	};

	deque() :
		elemCnt(0),
		last(new node()),
		needMaintain(new bool(false))
	{}

	deque(const deque &rhs) :
		elemCnt(rhs.elemCnt), 
		last(new node()),
		needMaintain(new bool(false))
	{
		//deep copy,只拷贝包含数据元素的node
		for (node *p = rhs.last->next; p != rhs.last; p = p->next)
		{
			if (p->validLen != 0)
				node::insert(new node(*p), last);
		}

		//整理
		maintain();
	}

	~deque()
	{
		clear();
		delete last;
		delete needMaintain;
	}

	deque& operator=(deque other)
	{
		exchange(other);
		return *this;
	}

	//access specified element with bounds checking
	T& at(size_t pos)
	{
		if (pos >= elemCnt)
			throw index_out_of_bound();

		if (*needMaintain)
		{
			maintain();
			*needMaintain = false;
		}

		return *(iterator(this, pos));
	}
    
	const T& at(size_t pos) const
	{
		if (pos >= elemCnt)
			throw index_out_of_bound();

		if (*needMaintain)
		{
			maintain();
			*needMaintain = false;
		}

		return *(const_iterator(this, pos));
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
		if (empty())
			throw container_is_empty();

		return *(cbegin());
	}

	//access the last element
	const T& back() const
	{
		if (empty())
			throw container_is_empty();

		return *(--cend());
	}

	//iterator to the beginning
	iterator begin()
	{ 
		if (*needMaintain)
		{
			maintain();
			*needMaintain = false;
		}

		return iterator(this, last->next, last->next->left);
	}

	const_iterator cbegin() const
	{ 
		if (*needMaintain)
		{
			maintain();
			*needMaintain = false;
		}

		return const_iterator(this, last->next, last->next->left);
	}

	//iterator to the end
	iterator end()
	{
		if (*needMaintain)
		{
			maintain();
			*needMaintain = false;
		}

		return iterator(this, last, nullptr);
	}

	const_iterator cend() const
	{ 
		if (*needMaintain)
		{
			maintain();
			*needMaintain = false;
		}

		return const_iterator(this, last, nullptr);
	}

    //true if the container is empty
	bool empty() const 
	{ 
		return size() == 0; 
	}

    //number of elements in the container
	size_t size() const 
	{ 
		return elemCnt; 
	}

    //release all the nodes, as well as the contents inside
	//also, relink the prev and next pointer of last to itself
	void clear()
	{
		node *p = last->next, *t = nullptr;
		while (p != last)
		{
			t = p;
			p = p->next;
			delete t;
		}
		last->prev = last->next = last;
		elemCnt = 0;
	}

	//inserts value before pos
	iterator insert(iterator pos, const T &value)
	{
		if (!pos.isValid(this))
			throw invalid_iterator();

		++elemCnt;

		if (pos.origin->validLen < pos.origin->totalLen)//当前node有空余
		{
			size_t leftMoveCnt = pos.cur - pos.origin->left;//[left,pos)之间元素个数
			size_t rightMoveCnt = pos.origin->validLen - leftMoveCnt;//[pos,left+validLen)之间元素个数
			size_t leftEmptyCnt = pos.origin->left - pos.origin->start;//左边空余的数量
			size_t rightEmptyCnt = pos.origin->totalLen - leftEmptyCnt - pos.origin->validLen;//右边空余的数量
			bool moveToLeft = leftEmptyCnt == 0 ? false : (rightMoveCnt == 0 ? true : leftMoveCnt <= rightMoveCnt);//确定移动方向

			//move elements
			if (moveToLeft)
			{
				T *dstStart = pos.origin->left - 1;
				T *srcStart = dstStart + 1;

				for (auto i = 0; i < leftMoveCnt; i++)
				{
					new (dstStart + i) T(*(srcStart + i));
					(srcStart + i)->~T();
				}

				--pos.origin->left;
				--pos.cur;
			}
			else
			{
				T *dstStart = pos.origin->left + pos.origin->validLen;
				T *srcStart = dstStart - 1;

				for (auto i = 0; i < rightMoveCnt; i++)
				{
					new (dstStart - i) T(*(srcStart - i));
					(srcStart - i)->~T();
				}
			}

			//insert new value
			new (pos.cur) T(value);
			++pos.origin->validLen;
			return pos;
		}
		else//当前node已满,split
		{
			node *p = pos.origin;
			const size_t ansIndex = pos.getIndex();
			//*needMaintain = true;

			size_t leftMoveCnt = pos.cur ? pos.cur - p->left : 0;//[left,pos)之间元素个数,处理了pos为end的情况
			size_t rightMoveCnt = p->validLen - leftMoveCnt;//[pos,left+validLen)之间元素个数
			size_t leftAllocCnt = leftMoveCnt + 1;
			size_t rightAllocCnt = rightMoveCnt;

			if (leftMoveCnt <= rightMoveCnt)//左半部分元素较少
			{
				node *tmp = new node(leftAllocCnt);
				node::insert(tmp, p);

				//拷贝[left,pos)的原有数据
				for (auto i = 0; i < leftMoveCnt; i++)
					new (tmp->left + i) T(*(p->left + i));

				//insert value
				tmp->validLen = leftAllocCnt;
				new (tmp->left + tmp->validLen - 1) T(value);
				
				//修正原node
				for (auto i = 0; i < leftMoveCnt; i++)
					(p->left + i)->~T();
				p->left = pos.cur;
				p->validLen -= leftMoveCnt;

				//return iterator(this, tmp, tmp->getBackPtr());
			}
			else//右半部分元素较少
			{
				node *tmp = new node(rightAllocCnt);
				node::insert_after(p, tmp);

				//拷贝[pos,left+validCnt)
				for (auto i = 0; i < rightMoveCnt; i++)
					new (tmp->left + i) T(*(pos.cur + i));
				tmp->validLen = rightAllocCnt;

				//清理
				for (auto i = 0; i < rightMoveCnt; i++)
					(pos.cur + i)->~T();

				//insert
				new (pos.cur) T(value);
				p->validLen -= (rightMoveCnt - 1);

				//return pos;
			}

			maintain();
			return  iterator(this, ansIndex);
		}
	}
	
	//removes specified element at pos
	iterator erase(iterator pos)
	{
		if (empty() || !pos.isValid(this) || pos == end())
			throw invalid_iterator();

		--elemCnt;
		node *p = pos.origin;

		if (p->validLen == 1)
		{
			node::link(p->prev, p->next);
			node *t = p->next;
			delete p;
			return iterator(this, t, t->left);
		}
		else
		{
			int leftMoveCnt = pos.cur - p->left;//[left,pos)之间的元素个数
			int rightMoveCnt = p->validLen - leftMoveCnt - 1;//(pos,left+validLen)之间的元素个数

			if (leftMoveCnt < rightMoveCnt)//左边元素较少，[left,pos)之间的元素向后move一个单位
			{
				T *dstStart = pos.cur;
				T *srcStart = dstStart - 1;

				for (auto i = 0; i < leftMoveCnt; i++)
				{
					(dstStart - i)->~T();
					new (dstStart - i) T(*(srcStart - i));
				}
				p->left->~T();

				++p->left;
				--p->validLen;

				++pos.cur;
				return pos;
			}
			else//右边元素较少，(pos,left+validLen)之间的元素向前move一个单位
			{
				T *dstStart = pos.cur;
				T *srcStart = dstStart + 1;

				for (auto i = 0; i < rightMoveCnt; i++)
				{
					(dstStart + i)->~T();
					new (dstStart + i) T(*(srcStart + i));
				}

				--p->validLen;
				(p->left + p->validLen)->~T();

				if (pos.cur == p->left + p->validLen)
				{
					do { p = p->next; } while (p != last && p->validLen == 0);
					return iterator(this, p, p->left);
				}
				else
					return pos;
			}
		}
	}

	//头尾操作
	void push_back(const T &value)
	{
		node *tmp = new node(1);
		new (tmp->left) T(value);
		++tmp->validLen;
		node::insert(tmp, last);
		++elemCnt;
		*needMaintain = true;
	}

	void pop_back()
	{
		erase(--end());
	}

	void push_front(const T &value)
	{
		node *tmp = new node(1);
		new (tmp->left) T(value);
		++tmp->validLen;
		node::insert_after(last, tmp);
		++elemCnt;
		*needMaintain = true;
	}

	void pop_front()
	{
		//erase不会改变needMaintain标志
		erase(begin());
	}

private:
	//根据下标返回iterator
	//要确保index合法！
	node* find(size_t &index) const
	{
		if (*needMaintain)
		{
			maintain();
			*needMaintain = false;
		}

		node *p = last->next;
		while (index >= p->validLen)
		{
			index -= p->validLen;
			p = p->next;
		}

		return p;
	}

	//将链表中较短的node合并
	void maintain() const
	{
		const size_t sn = (size_t)std::ceil(std::sqrt(elemCnt));

		node *p = last->next, *q = last->next, *r = last;
		size_t curCnt = 0;

		while (p != last)
		{
			//找到本次终结点
			while (q != last && curCnt + q->validLen <= sn)
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
					for (auto i = 0; i < t->validLen; i++)
						new (tmp->left + k++) T(*(t->left + i));
				}
			}
			tmp->validLen = curCnt;

			//删掉中间的零碎节点
			node *t = p, *c = nullptr;
			while (t != q)
			{
				c = t;
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
};

}

#endif
