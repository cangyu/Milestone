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

		//ͷ�����ÿռ�
		size_t leftAvailableCnt() const
		{
			return left - start;
		}

		//β�����ÿռ�
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
			if (pos.ascription.validLen < pos.ascription.totalLen)//��ǰnode�п���
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
					hasSplited = true;

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
		if (empty() || !pos.isValid(this) || pos == end())
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
		insert(begin(), value);
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

	//�������н϶̵�node�ϲ�
	void maintain()
	{

	}
};

}

#endif
