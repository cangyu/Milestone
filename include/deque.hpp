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

		~iterator() = default;
		
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

		//��n<0���ӵ�ǰλ�ÿ�ʼ��ǰ�ƶ�n����
		//	�����ǰλ����begin()���ƶ�������begin(),��throw invalid_iterator()��
		//	�����ǰλ����end()��originָ���deque�ǿ�,����ǰ�ƶ�һ��������β��Ԫ�ء�
		iterator& operator+=(int n)
		{
			if (n == 0)
				return *this;
			else if (n < 0)
				return operator-=(-n);
			else
			{
				//Syntax:����ƶ�n����Чstep�������ƶ�����������end(),��throw invalid_iterator()

				if (!cur)//ֻ��ָ��end()��iterator��cur�ſ�����nullptr
					throw invalid_iterator();

				int curRemainCnt = ascription->validLen - (cur - ascription->left) - 1;//��node��ʣ��Ŀɴӵ�ǰλ��ǰ���Ĳ���

				if (n <= curRemainCnt)//��ȻcurRemainCnt�п���Ϊ-1���������������else�������ݵ�
					cur += n;
				else
				{
					//�Ƶ���һ��node�ĵ�һ��Ԫ��
					node *p = ascription->next;
					n -= (curRemainCnt + 1);//��ʽ�ش�����curRemainCnt==-1�����

											//����move��n��ʾ��Ҫǰ���Ĳ���
					while (p != origin && n >= p->validLen)
					{
						n -= p->validLen;
						p = p->next;
					}

					//check�Ƿ��ƶ�����end()
					if (p == origin)
					{
						if (n>0)
							throw invalid_iterator();
						else
							cur = p->left;//nullptrֻ�������Ƚϣ����ܼ�0
					}
					else
						cur = p->left + n;

					ascription = p;
				}
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
				int frontLeftCnt = cur ? cur - ascription->left : 0;//����ָ�����end()����cur��Ϊnullptr

				if (n <= frontLeftCnt)
					cur -= n;
				else
				{
					n -= frontLeftCnt;
					node *p = ascription->prev;
					while (p != origin && n > p->validLen)
					{
						n -= p->validLen;
						p = p->prev;
					}

					//������ǰ�Ĺ�����������end(),���Ȼ�Ƿ���
					//�����˴�begin()��ʼ��dequeΪ�յ����
					if (p == origin)
						throw invalid_iterator();

					cur = p->left + p->validLen - n;
					ascription = p;
				}

				return *this;
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
			if (!ascription || !origin)//���Ա�ʶ���Ϸ�
				return false;

			if ((void*)origin!= id)//��������idָʾ��deque������ת�����ȼ�����˫Ŀ
				return false;
			
			if (ascription == origin)//end()
				return cur == nullptr;
			else
			{
				node *p = origin->next;
				while (p != origin && p != ascription)
					p = p->next;

				if (p == origin)//ascription������
					return false;
				else
				{
					if (p->left - cur > 0)//cur����left֮ǰ
						return false;
					else if (cur - p->left >= p->validLen)//cur������[left,left+validLen)
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
		friend class deque<T>;

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

		// return the distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const const_iterator &rhs) const
		{
			if (this->origin != rhs.origin)
				throw invalid_iterator();

			return rhs.getIndex() - this->getIndex();
		}
		
		//move n steps
		const_iterator& operator+=(int n)
		{
			if (n > 0)
			{
				if(!cur)//ֻ��ָ��end()��iterator��cur�ſ�����nullptr
					throw invalid_iterator();
				
				int curRemainCnt = ascription->validLen - (cur - ascription->left) - 1;//��node��ʣ��Ŀɴӵ�ǰλ��ǰ���Ĳ���

				if (n <= curRemainCnt)//��ȻcurRemainCnt�п���Ϊ-1���������������else�������ݵ�
					cur += n;
				else
				{
					//�Ƶ���һ��node�ĵ�һ��Ԫ��
					node *p = ascription->next;
					n-=(curRemainCnt+1);//��ʽ�ش�����curRemainCnt==-1�����
					
					//����move��n��ʾ��Ҫǰ���Ĳ���
					while (p!=origin && n >= p->validLen)
					{
						n -= p->validLen;
						p = p->next;
					}

					//check�Ƿ��ƶ�����end()
					if(p==origin)
					{
						if(n>0)
							throw invalid_iterator();
						else
							cur=p->left;//nullptrֻ�������Ƚϣ����ܼ�0
					}
					else
						cur = p->left + n;

					ascription = p;
				}
			}
			else if (n<0)
			{
				n = -n;
				int frontLeftCnt = cur ? cur-ascription->left : 0;//����ָ�����end()����cur��Ϊnullptr
				
				if (n <= frontLeftCnt)
					cur -= n;
				else
				{
					n -= frontLeftCnt;
					node *p = ascription->prev;
					while (p!=origin && n > p->validLen)
					{
						n -= p->validLen;
						p = p->prev;
					}
					
					//������ǰ�Ĺ�����������end(),���Ȼ�Ƿ���
					//�����˴�begin()��ʼ��dequeΪ�յ����
					if(p==origin)
						throw invalid_iterator();

					cur = p->left + p->validLen - n;
					ascription = p;
				}
			}

			return *this;
		}
		const_iterator& operator-=(int n)
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
            operator+=(1);
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
            operator-=(1);
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
			if (!ascription || !origin)//���Ա�ʶ���Ϸ�
				return false;

			if ((void*)origin != id)//��������idָʾ��deque������ת�����ȼ�����˫Ŀ
				return false;

			if (ascription == origin)//end()
				return cur == nullptr;
			else
			{
				node *p = origin->next;
				while (p != origin && p != ascription)
					p = p->next;

				if (p == origin)//ascription������
					return false;
				else
				{
					if (p->left - cur > 0)//cur����left֮ǰ
						return false;
					else if (cur - p->left >= p->validLen)//cur������[left,left+validLen)
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
		delete head;
		delete needMaintain;
	}

	deque& operator=(deque other)
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
		return elemCnt == 0 ? end() : find(0);
	}

	const_iterator cbegin() const
	{ 
		return elemCnt == 0 ? cend() : find(0);
	}

	//iterator to the end
	iterator end()
	{ 
		return iterator(nullptr, head, head); 
	}

	const_iterator cend() const
	{ 
		return const_iterator(nullptr, head, head);
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
	}

	//inserts value before pos
	iterator insert(iterator pos, const T &value)
	{
		if (!pos.isValid(head))
			throw invalid_iterator();

		++elemCnt;

		if (pos == end())
		{
			node *tmp = new node(1);
			new (tmp->left) T(value);
			++tmp->validLen;

			insert_before(head, tmp);
			*needMaintain = true;

			return iterator(tmp->left, tmp, head);
		}
		else
		{
			if (pos.ascription->validLen < pos.ascription->totalLen)//��ǰnode�п���
			{
				//[left,pos)֮��Ԫ�ظ���
				size_t leftMoveCnt = pos.cur - pos.ascription->left;
				//[pos,left+validLen)֮��Ԫ�ظ���
				size_t rightMoveCnt = pos.ascription->validLen - leftMoveCnt;

				//��߿��������
				size_t leftEmptyCnt = pos.ascription->left - pos.ascription->start;
				//�ұ߿��������
				size_t rightEmptyCnt = pos.ascription->totalLen - leftEmptyCnt - pos.ascription->validLen;

				//ȷ���ƶ�����
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

				//����ָ���²���Ԫ�ص�iterator
				return pos;
			}
			else//��ǰnode����
			{
				if (pos.cur == pos.ascription->left && pos.ascription->prev->rightAvailable())//����ͷ����ǰһ��node��β���п���
				{
					//����ǰһ��node��β��
					node *p = pos.ascription->prev;
					*(p->left + p->validLen) = value;
					++p->validLen;

					//����ָ��ǰһ��nodeβ��Ԫ�ص�iterator
					return iterator(p->left + p->validLen - 1, p, head);
				}
				else if (pos.cur == pos.ascription->getBackPtr() && pos.ascription->next->leftAvailable())//����β���Һ�һ��node��ͷ���п���
				{
					//���ں�һ��node��ͷ��
					node *p = pos.ascription->next;
					*(--p->left) = value;
					++p->validLen;

					//����ָ����һ��nodeͷ��Ԫ�ص�iterator
					return iterator(p->left, p, head);
				}
				else//split
				{
					node *p = pos.ascription;
					*needMaintain = true;

					//[left,pos]֮��Ԫ�ظ���
					size_t leftMoveCnt = pos.cur - p->left + 1;
					//[pos,left+validLen)֮��Ԫ�ظ���
					size_t rightMoveCnt = p->validLen - leftMoveCnt + 1;

					if (leftMoveCnt <= rightMoveCnt)//��벿��Ԫ�ؽ���
					{
						//[left,pos]
						node *tmp = new node(leftMoveCnt);

						//����[left,pos)��ԭ������
						for (int i = 0; i < leftMoveCnt - 1; i++)
							new (tmp->left + i) T(*(p->left + i));

						//insert value
						tmp->validLen = leftMoveCnt;
						new (tmp->left + tmp->validLen - 1) T(value);

						//����ԭnode
						p->left = pos.cur;
						p->validLen -= (leftMoveCnt - 1);

						//���µ�node����ԭnode֮ǰ
						insert_before(p, tmp);

						//����ָ���²����value��iterator����tmp��β��
						return iterator(tmp->getBackPtr(), tmp, head);
					}
					else//�Ұ벿��Ԫ�ؽ��٣�move them
					{
						//[pos,left+validCnt)
						node *tmp = new node(rightMoveCnt);

						//����[pos,left+validCnt)
						for (int i = 0; i < rightMoveCnt; i++)
							new (tmp->left + i) T(*(pos.cur + i));
						tmp->validLen = rightMoveCnt;

						//insert value
						*pos.cur = value;
						p->validLen -= (rightMoveCnt - 1);

						//���µ�node����ԭnode֮��
						insert_after(p, tmp);

						//����ָ���²����value��iterator����pos����
						return pos;
					}
				}
			}
		}
	}
	
	//removes specified element at pos
	iterator erase(iterator pos)
	{
		if (empty() || !pos.isValid(head) || pos == end())
			throw invalid_iterator();

		--elemCnt;
		if (pos.cur == pos.ascription->left + (pos.ascription->validLen - 1))//posָ��ǰnode�����һ������ʱԪ�ظ���>=1
		{
			--pos.ascription->validLen;

			//�ҵ���һ����Ϊ�յ�node
			node *p = pos.ascription->next;
			while (p != head && p->validLen == 0)
				p = p->next;

			//���ظ�node�ĵ�һ��Ԫ�أ���deque��ֻ��һ��Ԫ�أ�p���ص�head����end()
			return iterator(p->left, p, head);
		}
		else if (pos.cur == pos.ascription->left)//posָ��ǰnode�ĵ�һ������ʱԪ�ظ���>=2
		{
			++pos.ascription->left;
			--pos.ascription->validLen;

			//���ڲ������validLen==0�������ֱ�ӷ���ָ����һ��pos��iterator
			++pos.cur;
			return pos;
		}
		else//posָ��ǰnode�ķ�ͷβԪ�أ���ʱԪ�ظ���>=3
		{
			//pos֮ǰ��Ԫ�ظ���
			size_t numToMove = pos.cur - pos.ascription->left;

			if (2 * numToMove < pos.ascription->validLen)//ǰ�벿��Ԫ������һ��
			{
				//[left,pos)֮���Ԫ�����moveһ����λ
				std::memmove(pos.ascription->left + 1, pos.ascription->left, numToMove * sizeof(T));
				++pos.ascription->left;
				--pos.ascription->validLen;

				//����ָ��pos�����Ԫ�ص�iterator
				++pos.cur;
				return pos;
			}
			else
			{
				//[pos+1,left+validLen)֮���Ԫ����ǰmoveһ����λ
				numToMove = pos.ascription->validLen - numToMove - 1;
				std::memmove(pos.cur, pos.cur + 1, numToMove * sizeof(T));
				--pos.ascription->validLen;

				//�µ�Ԫ�������pos��ԭ����Ԫ�أ�ֱ�ӷ���
				return pos;
			}
		}
	}

	//ͷβ����
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
		//���ⲻ��Ҫ��maintain
		iterator tmp(head->next->left, head->next, head);
		insert(tmp, value);
	}

	void pop_front()
	{
		erase(begin());
	}

private:
	//��a�ڵ�֮ǰ����b�ڵ�
	static void insert_before(node *a, node *b)
	{
		b->next = a;
		a->prev->next = b;
		b->prev = a->prev;
		a->prev = b;
	}
	
	//��a�ڵ�֮�����b�ڵ�
	static void insert_after(node *a, node *b)
	{
		b->next = a->next;
		a->next = b;
		b->next->prev = b;
		b->prev = a;
	}

	//�����±귵��iterator
	//ȷ��index�Ϸ���
	iterator find(size_t index) const
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

		return iterator(p->left + index, p, head);
	}

	//�������н϶̵�node�ϲ�
	void maintain() const
	{
		const size_t sn = std::sqrt(elemCnt);

		node *p = head->next, *q = head->next, *r = head;
		size_t curCnt = 0;

		while (p != head)
		{
			//�ҵ������ս��
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
