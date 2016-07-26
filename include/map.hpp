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
		};

		size_t nodeCnt;
		rb_node* header;
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
		pair<iterator, bool> _insert(rb_node *x, rb_node *y, const ElemTypeDef& v)
		{

		}


	};
}

#endif