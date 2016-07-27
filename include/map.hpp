#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu 
{
	template<typename KeyTypeDef, typename ValTypeDef, class Compare = std::less<KeyTypeDef>>
	class map
	{
		typedef pair<const KeyTypeDef, ValTypeDef> value_type;
		class _getKeyFunctor
		{
		public:
			KeyTypeDef& operator() (const value_type& _elem) const
			{
				return _elem.first; 
			}
		};

		typedef rb_tree<KeyTypeDef, value_type, _getKeyFunctor, Compare> BalanceTreeTypeDef;

		typedef typename BalanceTreeTypeDef::iterator iterator;
		typedef typename BalanceTreeTypeDef::const_iterator const_iterator;

		BalanceTreeTypeDef *bt;

		iterator find(const KeyTypeDef& _key)
		{
			return bt->find(_key);
		}
		const_iterator find(const KeyTypeDef& _key) const
		{
			return bt->find(_key);
		}

		pair<iterator, bool> insert(const value_type &value) 
		{
			return bt->insert(value);
		}

		void erase(iterator pos)
		{
			bt->erase(pos);
		}

	};

	template<typename KeyTypeDef, typename ElemTypeDef, class GetKeyFunc, class Compare>
	class rb_tree
	{
	private:
		typedef enum { RED = 0, BLACK = 1 } ColorTypeDef;
		struct rb_node
		{
			ElemTypeDef elem;
			rb_node *parent, *left, *right;
			ColorTypeDef color;

			rb_node(const ElemTypeDef& _e, rb_node *_p = nullptr, rb_node *_l = nullptr, rb_node *_r = nullptr) :
				elem(_e),
				parent(_p),
				left(_l),
				right(_r),
				color(RED)
			{}
		};

		size_t nodeCnt;
		rb_node *header;
		Compare cmp;
		GetKeyFunc getKey;

	public:
		class const_iterator;
		class iterator
		{
		private:
			rb_node* node;
			rb_tree* ascription;

		public:
			iterator(rb_node* _nPtr = nullptr, rb_tree* _tPtr = nullptr) :node(_nPtr), ascription(_tPtr) {}
			iterator(const iterator& rhs) :node(rhs.node), ascription(rhs.ascription) {}

			ElemTypeDef& operator*() const { return node->elem; }
			ElemTypeDef* operator->() const noexcept { return &(operator*()); }

			iterator operator++(int) 
			{
				iterator tmp = *this;
				increment();
				return tmp;
			}
			iterator& operator++()
			{
				increment();
				return *this;
			}

			iterator operator--(int) 
			{
				iterator tmp = *this;
				decrement();
				return *this;
			}
			iterator& operator--() 
			{
				decrement();
				return *this;
			}

			bool operator==(const iterator &rhs) const 
			{
				return ascription == rhs.ascription && node == rhs.node;
			}
			bool operator==(const const_iterator &rhs) const 
			{
				return ascription == rhs.ascription && node == rhs.node;
			}

			bool operator!=(const iterator &rhs) const 
			{
				return !operator==(rhs);
			}
			bool operator!=(const const_iterator &rhs) const 
			{
				return !operator==(rhs);
			}

			bool isAscriptedTo(void* _id) const
			{
				return id && id == ascription;
			}
			bool isValid() const
			{
				if (ascription == nullptr || node == nullptr)
					return false;

				//检查是否指向了end()
				if (ascription->header && ascription->header == node)
					return false;

				return true;
			}
			
		private:
			void increment()
			{

			}

			void decrement()
			{

			}
		};

		class const_iterator
		{

		};

		iterator find(const KeyTypeDef& _key)
		{
			rb_node *cur = header->parent;
			rb_node *post = header;

			while (cur)
			{
				if (cmp(getKey(cur->elem), _key))
					cur = cur->right;
				else
				{
					post = cur;
					cur = cur->left;
				}
			}

			if (post == header || cmp(_key, getKey(post->elem)))
				return end();
			else
				return iterator(post, this);
		}

		pair<iterator, bool> insert(const ElemTypeDef& _val)
		{
			rb_node *y = header;
			rb_node *x = header->parent;//root
			bool cmpAns = true;

			while (x)
			{
				y = x;
				cmpAns = cmp(getKey(_val), getKey(x->elem));
				x = cmpAns ? x->left : x->right;
			}

			iterator j(y, this);
			if (cmpAns)
			{
				if (j == begin())
					return pair<iterator, bool>(_insert(x, y, _val), true);
				else
					--j;
			}
			if (cmp(getKey(j->elem), getKey(_val)))
				return pair<iterator, bool>(_insert(x, y, _val), true);

			return pair<iterator, bool>(j, false);
		}

		void erase(iterator pos)
		{
			if (!pos.isAscriptedTo(this) || !pos.isValid())
				throw invalid_iterator();

			_erase(pos);
		}

	private:
		iterator _insert(rb_node *x, rb_node *y, const ElemTypeDef& v)
		{
			rb_node *z = new rb_node(v, y);

			if (y == header || x || cmp(getKey(v), getKey(y->elem)))
			{
				y->left = z;
				if (y == header)
				{
					header->parent = z;
					header->left = z;
				}
				else if (y = header->left)
					header->left = z;
			}
			else
			{
				y->right = z;
				if (y == header->right)
					header->right = z;
			}

			_rebalance(z, header->parent);
			++nodeCnt;

			return iterator(z, this);
		}

		void _rebalance(rb_node* x, rb_node* &r)
		{
			x->color = RED;
			while (x != r && x->parent->color == RED)
			{
				if (x->parent == x->parent->parent->left)//父节点作为左儿子存在
				{
					rb_node *y = x->parent->parent->right;//x的伯父节点
					if (y && y->color == RED)//伯父节点也是RED，更换颜色再继续向上调整即可
					{
						x->parent->color = BLACK;
						y->color = BLACK;
						x->parent->parent->color = RED;
						x = x->parent->parent;//继续调整
					}
					else
					{
						if (x == x->parent->right)//LR，先做一个左旋转，转换成LL
						{
							x = x->parent;
							_left_rotate(x, r);
						}
						x->parent->color = BLACK;
						x->parent->parent->color = RED;
						_right_rotate(x->parent->parent, r);
					}
				}
				else//父节点作为右儿子存在
				{
					rb_node *y = x->parent->parent->left;//x的伯父节点
					if (y && y->color == RED)//同上
					{
						x->parent->color = BLACK;
						y->color = BLACK;
						x->parent->parent->color = RED;
						x = x->parent->parent;
					}
					else
					{
						if (x == x->parent->left)//RL，先做一个右旋转，转换成RR
						{
							x = x->parent;
							_right_rotate(x, r);
						}
						x->parent->color = BLACK;
						x->parent->parent->color = RED;
						_left_rotate(x->parent->parent, r);
					}
				}
			}
			r->color = BLACK;
		}

		void _left_rotate(rb_node* x, rb_node* &r)
		{
			rb_node *y = x->right;
			x->right = y->left;
			if (y->left)
				y->left->parent = x;
			y->parent = x->parent;

			if (x == r)
				r = y;
			else if (x == x->parent->left)
				x->parent->left = y;
			else
				x->parent->right = y;
			y->left = x;
			x->parent = y;
		}

		void _right_rotate(rb_node* x, rb_node* &r)
		{
			rb_node *y = x->left;
			x->left = y->right;
			if (y->right)
				y->right->left = x;
			y->parent = x->parent;

			if (x == r)
				r = y;
			else if (x == x->parent->left)
				x->parent->left = y;
			else
				x->parent->right = y;
			y->right = x;
			x->parent = y;
		}

		void _erase(iterator pos)
		{
			//TODO
		}
	};
}

#endif