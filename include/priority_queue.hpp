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
		//operator=的辅助函数
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
		//默认构造函数
		//不指定关键字值，其余均按指向自身设置，无孩子与双亲
		fibonacci_node():
			degree(0),
			left(this),
			right(this),
			child(nullptr),
			parent(nullptr),
			flag(false)
		{}

		//构造函数
		//指定关键字值，节点指向自身，无孩子与双亲
		fibonacci_node(const KeyTypeDef &_key) : 
			key(_key), 
			degree(0), 
			left(this), 
			right(this), 
			child(nullptr), 
			parent(nullptr), 
			flag(false) 
		{}

		//拷贝构造函数
		//不复制rhs的指针信息，只复制一个裸的节点，连接关系由调用函数设置
		fibonacci_node(const fibonacci_node &rhs) :
			key(rhs.key),
			degree(rhs.degree),
			left(this),
			right(this),
			child(nullptr),
			parent(nullptr),
			flag(rhs.flag)
		{}

		//析构函数
		//不处理连接关系，直接释放
		~fibonacci_node() {}

		//赋值运算
		//copy-on-swap idom
		fibonacci_node& operator=(fibonacci_node rhs)
		{
			exchange(rhs);
			return *this;
		}

		//在a之前插入b
		//a,b可以是一串双链表，也可以是单个节点，但要自身连接完好
		static void insert_before(fibonacci_node &a, fibonacci_node &b)
		{
			std::swap(a.left->right, b.left->right);
			std::swap(a.left, b.left);
		}

		//从双链表中移除节点t
		static fibonacci_node* remove_single(fibonacci_node* &t)
		{
			fibonacci_node* z = t;
			
			//更新左右两边的连接
			t->left->right = t->right;
			t->right->left = t->left;

			//更新原指针的指向
			if (t->right == t)
				t = nullptr;
			else
				t = t->right;

			//修正取出的节点的左右指针，指向自己
			z->left = z->right = z;

			return z;
		}

		//当前子树的深拷贝
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

		//释放当前子树
		void release()
		{
			while (child)
			{
				auto z = remove_single(child);
				z->release();
			}
			delete this;//需要自己删除自己。。。
		}

		//将_y链入_x的孩子列表
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

	//operator=的辅助函数
	void exchange(fibonacci_heap &rhs)
	{
		using std::swap;
		swap(n, rhs.n);
		swap(root, rhs.root);
		swap(cmp, rhs.cmp);
	}

	//复制一个fibonacci_heap到当前堆中
	//Copy constructor的辅助函数
	void duplicate(const fibonacci_heap &rhs)
	{
		auto w = rhs.root;
		do {
			auto z = w->copy();//拷贝该节点连同其子树，深拷贝
			insert(*z);
			w = w->right;
		} while (w != rhs.root);
	}

	//释放当前fibonacci_heap中的所有节点
	//Destructor的辅助函数
	void destroy()
	{
		while (root)//每remove_single后root的指向会自动改变
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
	//在根链表中插入一个节点,连同其子树
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

	//取出根节点
	fibonacci_node<T>* extract_root()
	{
		if (!root)
			return root;
		else
		{
			//若有孩子链表，则将其parent置为nullptr，并移入根链表
			if (root->child)
			{
				auto t = root->child;
				do {
					t->parent = nullptr;
					t = t->right;
				} while (t != root->child);

				fibonacci_node<T>::insert_before(*root, *root->child);
			}

			//取出根节点
			auto z= fibonacci_node<T>::remove_single(root);
			z->child = nullptr;
			n--;

			//合并根链表中重复度数的节点并更新root
			if (n > 1)
				consolidate();

			return z;
		}
	}

private:
	//合并根链表中有相同的度的节点
	//合并完成后更新root
	void consolidate()
	{
		double phi = 0.5*(1.0 + std::sqrt(5));//黄金分割率
		size_t num = std::ceil(std::log2(1.0*n) / std::log2(phi)) + 1;//D(n)<=floor(log2(n)/log2(phi))
		auto A = vector<fibonacci_node<T>*>(num, nullptr);

		//逐次取出根链表中的节点，并合并有相同degree的节点到A中
		while (root)
		{
			auto x = fibonacci_node<T>::remove_single(root);
			auto d = x->degree;

			while (A[d])
			{
				auto y = A[d];
				if ((*cmp)(x->key, y->key))
					std::swap(x, y);
				fibonacci_node<T>::link(*x, *y);//将y链入x的孩子列表
				A[d] = nullptr;
				++d;
			}
			A[d] = x;
		}

		//重构根链表
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
