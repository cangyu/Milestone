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
//������std::deque,�����ڿ�״����ʵ�֣����ö��Բ���ķ���
//insert��erase���ᴥ����ÿ��node��С�Ķ�̬����
//ֻ����access��ʱ��ſ��ܵ���ÿ��node��������sqrt(n)
//�������ֵ���ֻ���ڵ�һ��access���У��Ժ��access��������������
//����insert��erase�ľ�̯���Ӷ�ΪO(1)
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
		//Ĭ�Ϲ��죬������ռ�
		//ǰ��ָ�붼ָ���Լ�
		node() :
			prev(this),
			next(this),
			start(nullptr), 
			left(nullptr), 
			totalLen(0),
			validLen(0)
		{}

		//���������죬ֻ�����ڴ棬����ʼ��
		//ע�⣺��Ȼmalloc�Ĳ�������Ϊ0�����Ǿ�������
		node(size_t _len) :
			prev(this),
			next(this),
			start(nullptr),
			left(nullptr),
			totalLen(_len),
			validLen(0)
		{
			if (_len > 0)
				left = start = (T *)std::malloc(_len * sizeof(T));
		}

		//��������
		//ע�⣺ֻ��start��Ϊnullptrʱ����free
		~node()
		{
			if (start)
			{
				for (auto i = 0; i < validLen; i++)
					(left + i)->~T();

				std::free(start);
			}

			start = left = nullptr;
			totalLen = validLen = 0;
		}

		//�������죬ֻ������Ч�Ĳ���
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
				left = start = (T *)std::malloc(validLen * sizeof(T));
				for (auto i = 0; i < validLen; i++)
					new (start + i) T(*(rhs.left + i));
			}
		}

		//��ֵ��copy-on-swap
		node& operator=(node rhs)
		{
			exchange(rhs);
			return *this;
		}

		//�󲿷ֿ��ÿռ�
		size_t leftAvailableCnt() const
		{
			return left ? left - start : 0;
		}

		//�Ҳ��ֿ��ÿռ�
		size_t rightAvailableCnt() const
		{
			return left ? totalLen - leftAvailableCnt() - validLen : 0;
		}

		bool leftAvailable() const
		{
			return leftAvailableCnt() != 0;
		}

		bool rightAvailable() const
		{
			return rightAvailableCnt() != 0;
		}

		//���ÿռ�����һ��λ�õ�ָ��
		T* getBackPtr() const
		{
			return left ? left + validLen - 1 : nullptr;
		}

		//��b֮ǰ����a
		static void insert(node *a, node *b)
		{
			a->next = b;
			b->prev->next = a;
			a->prev = b->prev;
			b->prev = a;
		}

		//��a֮�����b
		static void insert_after(node *a, node *b)
		{
			b->next = a->next;
			a->next->prev = b;
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

		//������ǰ���ƶ�
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
				//Semantics:����ƶ�n����Чstep�������ƶ�����������end(),��throw invalid_iterator()
				while (origin != ascription->last)
				{
					//��node��ʣ��Ŀɴӵ�ǰλ��ǰ���Ĳ���,while ������֤��origin->left!=nullptr
					int curRemainCnt = origin->validLen - (cur - origin->left) - 1;

					if (n <= curRemainCnt)//�����ڱ�node�ڲ����
					{
						cur += n;
						return *this;
					}
					else//�ƶ�����һ��node�ĵ�һ��
					{
						n -= (curRemainCnt + 1);
						origin = origin->next;
						cur = origin->left;
					}
				}

				//������end()�Ҳ�Ϊ�յ㣬��invalid
				if (n != 0)
					throw invalid_iterator();
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
				//Semantics: �����ƶ�һ��֮������end(),��invalid�������ܿ�Խbegin()
				do
				{
					//ǰ��ʣ��Ŀ�ǰ���Ĳ���
					//ע�⣺nullptrֻ�����ڱȽϣ�������������
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

				//���е��˴���Ȼ�ǿ�Խ��begin
				throw invalid_iterator();
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

		//key utilities of an iterator
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
			if (!ascription || !rhs.ascription || ascription != rhs.ascription)
				return false;
			else if (!origin || !rhs.origin || origin != rhs.origin)
				return false;
			else
				return cur == rhs.cur;//����ָ��end()����ʱcurΪnullptr
		}

		bool operator==(const const_iterator &rhs) const
		{
			if (!ascription || !rhs.ascription || ascription != rhs.ascription)
				return false;
			else if (!origin || !rhs.origin || origin != rhs.origin)
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
			//���ascription�Ƿ�һ��
			if (!id || id != ascription) 
				return false;
			
			//OK, they now point to the same deque.
			//���origin�Ƿ����
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
			//���cur��Χ�Ƿ�valid
			if (cur == nullptr)
				return origin->left == nullptr;//������end()
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
		T *cur;

		void exchange(const_iterator &rhs)
		{
			std::swap(ascription, rhs.ascription);
			std::swap(origin, rhs.origin);
			std::swap(cur, rhs.cur);
		}

	public:
		//constructor
		const_iterator(const deque<T> *_a = nullptr, node *_ori = nullptr, T *_cur = nullptr) :
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

		//������ǰ���ƶ�
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
				//Semantics:����ƶ�n����Чstep�������ƶ�����������end(),��throw invalid_iterator()
				while (origin != ascription->last)
				{
					//��node��ʣ��Ŀɴӵ�ǰλ��ǰ���Ĳ���,while ������֤��origin->left!=nullptr
					int curRemainCnt = origin->validLen - (cur - origin->left) - 1;

					if (n <= curRemainCnt)//�����ڱ�node�ڲ����
					{
						cur += n;
						return *this;
					}
					else//�ƶ�����һ��node�ĵ�һ��
					{
						n -= (curRemainCnt + 1);
						origin = origin->next;
						cur = origin->left;
					}
				}

				//������end()�Ҳ�Ϊ�յ㣬��invalid
				if (n != 0)
					throw invalid_iterator();
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
				//Semantics: �����ƶ�һ��֮������end(),��invalid�������ܿ�Խbegin()
				do
				{
					//ǰ��ʣ��Ŀ�ǰ���Ĳ���
					//ע�⣺nullptrֻ�����ڱȽϣ�������������
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

				//���е��˴���Ȼ�ǿ�Խ��begin
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
			return *cur;
		}

		const T* operator->() const noexcept
		{
			return &(operator*());
		}

		//check if two iterators pointing to same memory
		bool operator==(const iterator &rhs) const
		{
			if (!ascription || !rhs.ascription || ascription != rhs.ascription)
				return false;
			else if (!origin || !rhs.origin || origin != rhs.origin)
				return false;
			else
				return cur == rhs.cur;//����ָ��end()����ʱcurΪnullptr
		}

		bool operator==(const const_iterator &rhs) const
		{
			if (!ascription || !rhs.ascription || ascription != rhs.ascription)
				return false;
			else if (!origin || !rhs.origin || origin != rhs.origin)
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
			//���ascription�Ƿ�һ��
			if (!id || id != ascription)
				return false;

			//OK, they now point to the same deque.
			//���origin�Ƿ����
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
			//���cur��Χ�Ƿ�valid
			if (cur == nullptr)
				return origin->left == nullptr;//������end()
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
		needMaintain(new bool(true))
	{}

	deque(const deque &rhs) :
		elemCnt(rhs.elemCnt), 
		last(new node()),
		needMaintain(new bool(true))
	{
		//deep copy
		//ֻ������������Ԫ�ص�node
		for (node *p = rhs.last->next; p != rhs.last; p = p->next)
		{
			if (p->validLen != 0)
				node::insert(new node(*p), last);
		}

		//�������ı�needMaintain��ʶ��
		//��ֹ�к�����push_front or push_back
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

		node *p = find(pos);
		return *(p->left + pos);
	}
    
	const T& at(size_t pos) const
	{
		if (pos >= elemCnt)
			throw index_out_of_bound();

		node *p = find(pos);
		return *(p->left + pos);
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

		return *(cbegin());
	}

	//access the last element
	const T& back() const
	{
		if (elemCnt == 0)
			throw container_is_empty();

		return *(--cend());
	}

	//iterator to the beginning
	iterator begin()
	{ 
		if (empty())
			return end();
		else
		{
			size_t target = 0;
			node *p = find(target);
			return iterator(this, p, p->left + target);
		}
	}

	const_iterator cbegin() const
	{ 
		if (empty())
			return cend();
		else
		{
			size_t target = 0;
			node *p = find(target);
			return const_iterator(this, p, p->left + target);
		}
	}

	//iterator to the end
	iterator end()
	{ 
		return iterator(this, last, nullptr);
	}

	const_iterator cend() const
	{ 
		return const_iterator(this, last, nullptr);
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

		if (pos == end())
		{
			node *tmp = new node(1);
			new (tmp->left) T(value);
			++tmp->validLen;

			node::insert(tmp, last);
			*needMaintain = true;

			return iterator(this, tmp, tmp->left);
		}
		else if (pos.origin->validLen < pos.origin->totalLen)//��ǰnode�п���
		{
			size_t leftMoveCnt = pos.cur - pos.origin->left;//[left,pos)֮��Ԫ�ظ���
			size_t rightMoveCnt = pos.origin->validLen - leftMoveCnt;//[pos,left+validLen)֮��Ԫ�ظ���
			size_t leftEmptyCnt = pos.origin->left - pos.origin->start;//��߿��������
			size_t rightEmptyCnt = pos.origin->totalLen - leftEmptyCnt - pos.origin->validLen;//�ұ߿��������
			bool moveToLeft = leftEmptyCnt == 0 ? false : (rightMoveCnt == 0 ? true : leftMoveCnt <= rightMoveCnt);//ȷ���ƶ�����

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
		else//��ǰnode����
		{
			if (pos.cur == pos.origin->left && pos.origin->prev->rightAvailable())//����ͷ����ǰһ��node��β���п���
			{
				//����ǰһ��node��β��
				node *p = pos.origin->prev;
				new (p->left + p->validLen) T(value);
				++p->validLen;

				//����ָ��ǰһ��nodeβ��Ԫ�ص�iterator
				return iterator(this, p, p->left + p->validLen - 1);
			}
			else if (pos.cur == pos.origin->getBackPtr() && pos.origin->next->leftAvailable())//����β���Һ�һ��node��ͷ���п���
			{
				//���ں�һ��node��ͷ��
				node *p = pos.origin->next;
				--p->left;
				new (p->left) T(value);
				++p->validLen;

				//����ָ����һ��nodeͷ��Ԫ�ص�iterator
				return iterator(this, p, p->left);
			}
			else//split
			{
				node *p = pos.origin;
				*needMaintain = true;

				size_t leftMoveCnt = pos.cur - p->left;//[left,pos)֮��Ԫ�ظ���
				size_t rightMoveCnt = p->validLen - leftMoveCnt;//[pos,left+validLen)֮��Ԫ�ظ���
				size_t leftAllocCnt = leftMoveCnt + 1;
				size_t rightAllocCnt = rightMoveCnt;

				if (leftMoveCnt <= rightMoveCnt)//��벿��Ԫ�ؽ���
				{
					node *tmp = new node(leftAllocCnt);
					node::insert(tmp, p);

					//����[left,pos)��ԭ������
					for (int i = 0; i < leftMoveCnt; i++)
						new (tmp->left + i) T(*(p->left + i));

					//insert value
					tmp->validLen = leftAllocCnt;
					new (tmp->left + tmp->validLen - 1) T(value);

					//����ԭnode
					for (auto i = 0; i < leftMoveCnt; i++)
						(p->left + i)->~T();
					p->left = pos.cur;
					p->validLen -= leftMoveCnt;

					return iterator(this, tmp, tmp->getBackPtr());
				}
				else//�Ұ벿��Ԫ�ؽ���
				{
					node *tmp = new node(rightAllocCnt);
					node::insert_after(p, tmp);

					//����[pos,left+validCnt)
					for (auto i = 0; i < rightMoveCnt; i++)
						new (tmp->left + i) T(*(pos.cur + i));
					tmp->validLen = rightAllocCnt;

					//����
					for (auto i = 0; i < rightMoveCnt; i++)
						(pos.cur + i)->~T();

					//insert
					new (pos.cur) T(value);
					p->validLen -= (rightMoveCnt - 1);

					return pos;
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
		node *p = pos.origin;

		if (pos.cur == p->getBackPtr())//posָ��ǰnode�����һ������ʱԪ�ظ���>=1
		{
			--p->validLen;
			(p->left + p->validLen)->~T();

			//�ҵ���һ����Ϊ�յ�node
			node *t = p->next;
			while (t != last && t->validLen == 0)
				t = t->next;

			return iterator(this, t, t->left);
		}
		else if (pos.cur == p->left)//posָ��ǰnode�ĵ�һ������ʱԪ�ظ���>=2
		{
			p->left->~T();
			++p->left;
			--p->validLen;

			//���ڲ������validLen==0�������ֱ�ӷ���ָ����һ��pos��iterator
			++pos.cur;
			return pos;
		}
		else//posָ��ǰnode�ķ�ͷβԪ�أ���ʱԪ�ظ���>=3
		{
			size_t numToMove = pos.cur - p->left;//pos֮ǰ��Ԫ�ظ���

			if (2 * numToMove <= p->validLen)//ǰ�벿��Ԫ������һ��
			{
				//[left,pos)֮���Ԫ�����moveһ����λ
				T *dstStart = pos.cur;
				T *srcStart = dstStart - 1;

				for (auto i = 0; i < numToMove; i++)
				{
					(dstStart - i)->~T();
					new (dstStart - i) T(*(srcStart - i));
				}
				p->left->~T();

				++p->left;
				--p->validLen;

				//����ָ��pos�����Ԫ�ص�iterator
				++pos.cur;
				return pos;
			}
			else
			{
				//[pos+1,left+validLen)֮���Ԫ����ǰmoveһ����λ
				numToMove = p->validLen - numToMove - 1;

				T *dstStart = pos.cur;
				T *srcStart = dstStart + 1;

				for (auto i = 0; i < numToMove; i++)
				{
					(dstStart + i)->~T();
					new (dstStart + i) T(*(srcStart + i));
				}

				--p->validLen;
				(p->left + p->validLen)->~T();

				//�µ�Ԫ�������pos��ԭ����Ԫ�أ�ֱ�ӷ���
				return pos;
			}
		}
	}

	//ͷβ����
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
		//erase����ı�needMaintain��־
		erase(begin());
	}

private:
	//�����±귵��iterator
	//Ҫȷ��index�Ϸ���
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

	//�������н϶̵�node�ϲ�
	void maintain() const
	{
		const size_t sn = (size_t)std::ceil(std::sqrt(elemCnt));

		node *p = last->next, *q = last->next, *r = last;
		size_t curCnt = 0;

		while (p != last)
		{
			//�ҵ������ս��
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

			//ɾ���м������ڵ�
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
