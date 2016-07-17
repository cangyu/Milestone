#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <algorithm>
#include <cstdlib>
#include <cstddef>
#include <functional>
#include "exceptions.hpp"

#include "vector.hpp"

namespace sjtu 
{
//A container like std::priority_queue which is a heap internal.
template<typename T, class Compare = std::less<T>, typename HeapTypeDef = fibonacci_heap<T, Compare> >
class priority_queue 
{
private:
	HeapTypeDef *heap;

	void swap(priority_queue &other) { std::swap(heap, other.heap); }

public:
	/**
	 * constructors
	 */
	priority_queue():heap(new HeapTypeDef()) {}
	priority_queue(const priority_queue &other) :heap(new HeapTypeDef(*(other.heap))) {}

	/**
	 * deconstructor
	 */
	~priority_queue() { delete heap; }

	/**
	 * Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) 
	{
		priority_queue tmp(other);
		swap(tmp);
		return *this;
	}

	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const 
	{
		if (empty())
			throw container_is_empty();

		return heap->front();
	}

	/**
	 * push new element to the priority queue.
	 */
	void push(const T &e) { heap->push_back(e); }
	
	/**
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() 
	{
		if (empty())
			throw container_is_empty();

		heap->pop_front();
	}

	/**
	 * return the number of the elements.
	 */
	size_t size() const { return heap->size(); }

	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const { return heap->empty(); }
};

//A binary heap for the internal heap of a priority_queue.
//The implementation is based on the vector implemented before.
template<typename T, class Compare = std::less<T> >
class binary_heap
{
private:
	sjtu::vector<T> c;
	Compare cmp;

	size_t parent(size_t i) const { return (i - 1) / 2; }
	size_t left(size_t i) const { return 2 * i + 1; }
	size_t right(size_t i) const { return 2 * i + 2; }

	void percolateDown(size_t pos)
	{
		const size_t n = c.size();

		size_t child = left(pos);
		while (child < n)
		{
			if (child + 1 < n && cmp(c[child], c[child+1]))
				++child;

			if (cmp(c[child], c[pos]))
				break;
			else
			{
				std::swap(c[child], c[pos]);
				pos = child;
				child = left(pos);
			}
		}
	}

	void percolateUp(size_t pos)
	{
		size_t father = 0;
		while (pos > 0)
		{
			father = parent(pos);
			if (cmp(c[pos], c[father]))
				break;
			else
			{
				std::swap(c[pos], c[father]);
				pos = father;
				father = parent(pos);
			}
		}
	}

public:
	binary_heap() {}
	binary_heap(const Compare &x):cmp(x) {}

	binary_heap(const binary_heap &other) :c(other.c),cmp(other.cmp) {}

	~binary_heap() {}

	bool empty() const { return c.empty(); }

	size_t size() const { return c.size(); }

	const T &front() const
	{
		if (c.empty())
			throw container_is_empty();

		return c.front();
	}

	void push_back(const T &elem)
	{
		size_t pos = c.size();
		c.push_back(elem);
		percolateUp(pos);
	}

	void pop_front()
	{
		std::swap(c[0], c[size() - 1]);	
		c.pop_back();
		percolateDown(0);

	}
};

//A fibonacci heap for the internal heap of a priority_queue.
//Obviously, this data structure differs from binary heap as it supports the merge operation.
template<typename T, class Compare = std::less<T> >
class fibonacci_heap
{
private:
	template<typename KeyTypeDef>
	class fibonacci_node
	{
	public:
		KeyTypeDef key;
		size_t degree;
		fibonacci_node *left, *right, *child, *parent;
		bool flag;

	private:
		//operator=�ĸ�������
		void exchange(fibonacci_node &rhs)
		{
			using std::swap;
			swap(key, rhs.key);
			swap(degree, rhs.degree);
			swap(left, rhs.left);
			swap(right, rhs.right);
			swap(child, rhs.child);
			swap(parent, rhs.parent);
			swap(flag, rhs.flag);
		}

	public:
		//Ĭ�Ϲ��캯��
		//��ָ���ؼ���ֵ���������ָ���������ã��޺�����˫��
		fibonacci_node():
			degree(0),
			left(this),
			right(this),
			child(nullptr),
			parent(nullptr),
			flag(false)
		{}

		//���캯��
		//ָ���ؼ���ֵ���ڵ�ָ�������޺�����˫��
		fibonacci_node(const KeyTypeDef &_key) : 
			key(_key), 
			degree(0), 
			left(this), 
			right(this), 
			child(nullptr), 
			parent(nullptr), 
			flag(false) 
		{}

		//�������캯��
		//������rhs��ָ����Ϣ��ֻ����һ����Ľڵ㣬���ӹ�ϵ�ɵ��ú�������
		fibonacci_node(const fibonacci_node &rhs) :
			key(rhs.key),
			degree(rhs.degree),
			left(this),
			right(this),
			child(nullptr),
			parent(nullptr),
			flag(rhs.flag)
		{}

		//��������
		//���������ӹ�ϵ��ֱ���ͷ�
		~fibonacci_node() {}

		//��ֵ����
		//copy-on-swap idom
		fibonacci_node& operator=(fibonacci_node rhs)
		{
			exchange(rhs);
			return *this;
		}

		//��a֮ǰ����b
		//a,b������һ��˫����Ҳ�����ǵ����ڵ㣬��Ҫ�����������
		static void insert_before(fibonacci_node &a, fibonacci_node &b)
		{
			std::swap(a.left->right, b.left->right);
			std::swap(a.left, b.left);
		}

		//��˫�������Ƴ��ڵ�t
		static fibonacci_node* remove_single(fibonacci_node* &t)
		{
			fibonacci_node* z = t;
			
			//�����������ߵ�����
			t->left->right = t->right;
			t->right->left = t->left;

			//����ԭָ���ָ��
			if (t->right == t)
				t = nullptr;
			else
				t = t->right;

			//����ȡ���Ľڵ������ָ�룬ָ���Լ�
			z->left = z->right = z;

			return z;
		}

		//��ǰ���������
		fibonacci_node* copy() const
		{
			fibonacci_node* z = new fibonacci_node(*this);
			if (child)
			{
				auto w = child;
				do {
					auto cur_child_copy = w->copy();
					cur_child_copy->parent = z;

					if (z->child == nullptr)
						z->child = cur_child_copy;
					else
						insert_before(*z->child, *cur_child_copy);

					w = w->right;
				} while (w != child);
			}
			return z;
		}

		//�ͷŵ�ǰ����
		void release()
		{
			while (child)
			{
				auto z = remove_single(child);
				z->release();
			}
			delete this;//��Ҫ�Լ�ɾ���Լ�������
		}

		//��_y����_x�ĺ����б�
		static void link(fibonacci_node &_x, fibonacci_node &_y)
		{
			if (_x.child)
				insert_before(*_x.child, _y);
			else
				_x.child = &_y;

			++_x.degree;
			_y.flag = false;
		}
	};

private:
	size_t n;
	fibonacci_node<T> *root;
	Compare *cmp;

	//operator=�ĸ�������
	void exchange(fibonacci_heap &rhs)
	{
		using std::swap;
		swap(n, rhs.n);
		swap(root, rhs.root);
		swap(cmp, rhs.cmp);
	}

	//����һ��fibonacci_heap����ǰ����
	//Copy constructor�ĸ�������
	void duplicate(const fibonacci_heap &rhs)
	{
		auto w = rhs.root;
		do {
			auto z = w->copy();//�����ýڵ���ͬ�����������
			insert(*z);
			w = w->right;
		} while (w != rhs.root);
	}

	//�ͷŵ�ǰfibonacci_heap�е����нڵ�
	//Destructor�ĸ�������
	void destroy()
	{
		while (root)//ÿremove_single��root��ָ����Զ��ı�
		{
			auto z = fibonacci_node<T>::remove_single(root);
			z->release();
		}
		n = 0;
	}

public:
	//Default constructor
	fibonacci_heap() :n(0), root(nullptr), cmp(new Compare()) {}

	//Copy constructor 
	fibonacci_heap(const fibonacci_heap &rhs) :n(rhs.n), root(nullptr), cmp(new Compare(*rhs.cmp))
	{
		duplicate(rhs); 
	}

	//Destructor
	~fibonacci_heap() 
	{
		delete cmp;
		destroy();
	}

	//Assignment operator
	fibonacci_heap& operator=(fibonacci_heap rhs)
	{
		exchange(rhs);
		return *this;
	}

	//Get number of fibonacci_node in current heap.
	size_t size() const
	{ 
		return n;
	}

	//Check empty or not.
	bool empty() const 
	{ 
		return n == 0;
	}

	//Get the key of the root node.
	const T& front() const 
	{ 
		return root->key;
	}

	//Push a new fibonacci_node into current heap with given key.
	void push_back(const T &_key) 
	{ 
		auto tmp = new fibonacci_node<T>(_key);
		insert(*tmp);
		n++;
	}

	//Pop root node and release it.
	void pop_front()
	{ 
		auto tmp= extract_root();
		delete tmp;
	}

private:
	//�ڸ������в���һ���ڵ�,��ͬ������
	void insert(fibonacci_node<T> &x)
	{
		if (!root)
			root = &x;
		else
		{
			fibonacci_node<T>::insert_before(*root, x);
			if ((*cmp)(root->key, x.key))
				root = &x;
		}
	}

	//ȡ�����ڵ�
	fibonacci_node<T>* extract_root()
	{
		if (!root)
			return root;
		else
		{
			//���к�����������parent��Ϊnullptr�������������
			if (root->child)
			{
				auto t = root->child;
				do {
					t->parent = nullptr;
					t = t->right;
				} while (t != root->child);

				fibonacci_node<T>::insert_before(*root, *root->child);
			}

			//ȡ�����ڵ�
			auto z= fibonacci_node<T>::remove_single(root);
			z->child = nullptr;
			n--;

			//�ϲ����������ظ������Ľڵ㲢����root
			if (n > 1)
				consolidate();

			return z;
		}
	}

private:
	//�ϲ�������������ͬ�ĶȵĽڵ�
	//�ϲ���ɺ����root
	void consolidate()
	{
		double phi = 0.5*(1.0 + std::sqrt(5));//�ƽ�ָ���
		size_t num = std::ceil(std::log2(1.0*n) / std::log2(phi)) + 1;//D(n)<=floor(log2(n)/log2(phi))
		auto A = vector<fibonacci_node<T>*>(num, nullptr);

		//���ȡ���������еĽڵ㣬���ϲ�����ͬdegree�Ľڵ㵽A��
		while (root)
		{
			auto x = fibonacci_node<T>::remove_single(root);
			auto d = x->degree;

			while (A[d])
			{
				auto y = A[d];
				if ((*cmp)(x->key, y->key))
					std::swap(x, y);
				fibonacci_node<T>::link(*x, *y);//��y����x�ĺ����б�
				A[d] = nullptr;
				++d;
			}
			A[d] = x;
		}

		//�ع�������
		for (auto i = 0; i < A.size(); i++)
		{
			if (A[i])
			{
				if (root == nullptr)
					root = A[i];
				else
				{
					fibonacci_node<T>::insert_before(*root, *A[i]);
					if ((*cmp)(root->key, A[i]->key))
						root = A[i];
				}
			}
		}
	}
};
}
#endif
