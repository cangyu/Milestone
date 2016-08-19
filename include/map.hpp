#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

#include <functional>// only for std::less<T>
#include <cstddef>
#include <cstdlib>

#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu 
{
    //Internal balance tree for map.
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

			//默认颜色为red，指针为nullptr
			rb_node(const ElemTypeDef& _e, rb_node *_p = nullptr, rb_node *_l = nullptr, rb_node *_r = nullptr, ColorTypeDef _c = RED) :
				elem(_e),
				parent(_p),
				left(_l),
				right(_r),
				color(_c)
			{}

			//拷贝构造函数，只复制对方的elem和color，指针设为nullptr
			rb_node(const rb_node &rhs) :
				elem(rhs.elem),
				parent(nullptr),
				left(nullptr),
				right(nullptr),
				color(rhs.color)
			{}

			static rb_node* _minimum(rb_node* x)
			{
				while (x->left)
					x = x->left;
				return x;
			}

			static rb_node* _maximum(rb_node* x)
			{
				while (x->right)
					x = x->right;
				return x;
			}
		};

		size_t nodeCnt;
		rb_node *header;
		Compare cmp;
		GetKeyFunc getKey;

		void exchange(rb_tree &rhs)
		{
			std::swap(nodeCnt, rhs.nodeCnt);
			std::swap(header, rhs.header);
			std::swap(cmp, rhs.cmp);
			std::swap(getKey, rhs.getKey);
		}

	public:
		class const_iterator;
		class iterator
		{
			friend class const_iterator;
			friend class rb_tree<KeyTypeDef, ElemTypeDef, GetKeyFunc, Compare>;

		private:
			rb_node* node;
			rb_tree* ascription;
            
            void exchange(iterator &rhs)
            {
                std::swap(node, rhs.node);
                std::swap(ascription, rhs.ascription);
            }

		public:
			iterator(rb_node* _nPtr = nullptr, rb_tree* _tPtr = nullptr) :
                node(_nPtr), 
                ascription(_tPtr) 
            {}
            
			iterator(const iterator& rhs) :
                node(rhs.node), 
                ascription(rhs.ascription) 
            {}
            
            iterator& operator=(iterator rhs)
            {
                exchange(rhs);
                return *this;
            }
            
			~iterator() = default;

			ElemTypeDef& operator*() const 
            { 
                return node->elem; 
            }
			
            ElemTypeDef* operator->() const noexcept 
            { 
                return &(operator*()); 
            }

			iterator operator++(int)
			{
				iterator tmp(*this);
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
				iterator tmp(*this);
				decrement();
				return tmp;
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
				return _id && _id == ascription;
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
			//若iterator指向二叉序最后一个元素，increment后是header
			//若iterator指向header，incerment后是最后一个元素的最左node，这个行为是未定义的,但是在rb_tree为空时，会造成死循环,throw
			void increment()
			{
				if (node == ascription->header)
					throw invalid_iterator();

				if (node->right)
				{
					node = node->right;
					while (node->left)
						node = node->left;
				}
				else
				{
					rb_node *y = node->parent;
					while (node == y->right)
					{
						node = y;
						y = y->parent;
					}

					//Corner case：node原先指向root且root没有右孩子，所以经过上面的while后node在header，y在root，这时不能将node设为y
					if (node->right != y)
						node = y;
				}
			}

			//若iterator指向二叉序第一个元素，decrement未定义，直接throw
			//若iterator指向header，decrement后是最后一个rb_node
			void decrement()
			{
				if(node==ascription->header->left)
					throw invalid_iterator();

				if (node->color == RED && node->parent->parent == node)//node有可能指向header，因为header和root都具有经过两个连续的parent后指向自己的性质，还要用颜色来限定确保是header
					node = node->right;
				else if (node->left)
				{
					node = node->left;
					while (node->right)
						node = node->right;
				}
				else
				{
					rb_node *y = node->parent;
					while (node == y->left)
					{
						node = y;
						y = y->parent;
					}
					node = y;//若node指向第一个节点，则--后指向header
				}
			}
		};

		class const_iterator
		{
			friend class iterator;
			friend class rb_tree<KeyTypeDef, ElemTypeDef, GetKeyFunc, Compare>;

		private:
			const rb_node* node;
			const rb_tree* ascription;
            
            void exchange(const_iterator &rhs)
            {
                std::swap(node, rhs.node);
                std::swap(ascription, rhs.ascription);
            }

		public:
			const_iterator(const rb_node* _nPtr = nullptr, const rb_tree* _tPtr = nullptr) :
                node(_nPtr), 
                ascription(_tPtr) 
            {}
            
			const_iterator(const const_iterator& rhs) :
                node(rhs.node), 
                ascription(rhs.ascription) 
            {}
            
			const_iterator(const iterator& rhs) :
                node(rhs.node), 
                ascription(rhs.ascription) 
            {}
            
            const_iterator& operator=(const_iterator rhs)
            {
                exchange(rhs);
                return *this;
            }
            
			~const_iterator() = default;

			const ElemTypeDef& operator*() const 
            {   
                return node->elem;
            }
			
            const ElemTypeDef* operator->() const noexcept //返回得到的指针可以被修改，但是指针指向的内容不能被修改
            { 
                return &(operator*()); 
            }

			const_iterator operator++(int)
			{
				const_iterator tmp(*this);
				increment();
				return tmp;
			}
			
			const const_iterator& operator++()
			{
				increment();
				return *this;
			}

			const_iterator operator--(int)
			{
				const_iterator tmp(*this);
				decrement();
				return *this;
			}

			const const_iterator& operator--()
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
				return _id && _id == ascription;
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
				if (node == ascription->header)
					throw invalid_iterator();

				if (node->right)
				{
					node = node->right;
					while (node->left)
						node = node->left;
				}
				else
				{
					rb_node *y = node->parent;
					while (node == y->right)
					{
						node = y;
						y = y->parent;
					}

					//Corner case：node原先指向root且root没有右孩子，所以经过上面的while后node在header，y在root，这时不能将node设为y
					if (node->right != y)
						node = y;
				}
			}
            
			void decrement()
			{
				if (node == ascription->header->left)
					throw invalid_iterator();

				if (node->color == RED && node->parent->parent == node)//node有可能指向header，因为header和root都具有经过两个连续的parent后指向自己的性质，还要用颜色来限定确保是header
					node = node->right;
				else if (node->left)
				{
					node = node->left;
					while (node->right)
						node = node->right;
				}
				else
				{
					rb_node *y = node->parent;
					while (node == y->left)
					{
						node = y;
						y = y->parent;
					}

					node = y;//若node指向第一个节点，则--后指向header
				}
			}
		};

		rb_tree() :
			nodeCnt(0),
			header((rb_node *)std::malloc(sizeof(rb_node))),
			cmp(),
			getKey()
		{
            header->parent=nullptr;
			header->right = header->left = header;
            header->color=RED;
		}

		rb_tree(const rb_tree& rhs) :
			nodeCnt(rhs.nodeCnt),
			header((rb_node *)std::malloc(sizeof(rb_node))),
			cmp(rhs.cmp),
			getKey(rhs.getKey)
		{
            header->parent=nullptr;
			header->right = header->left = header;
            header->color=RED;
            
			header->parent = copyTree(rhs.header->parent);
			if (nodeCnt)
			{
				header->parent->parent = header;
				header->left = rb_node::_minimum(header->parent);
				header->right = rb_node::_maximum(header->parent);
			}
		}

		rb_tree& operator=(rb_tree rhs)
		{
			exchange(rhs);
			return *this;
		}

		~rb_tree()
		{
			clear();
            
            std::free(header);
			header = nullptr;
		}

		iterator begin()
		{
			return iterator(header->left, this);
		}
        
		const_iterator cbegin() const
		{
			return const_iterator(header->left, this);
		}

		iterator end()
		{
			return iterator(header, this);
		}
        
		const_iterator cend() const
		{
			return const_iterator(header, this);
		}

		bool empty() const
		{
			return size() == 0;
		}

		size_t size() const
		{
			return nodeCnt;
		}

		void clear()
		{
			makeEmpty(header->parent);
			nodeCnt = 0;

			header->parent = nullptr;
			header->right = header->left = header;
		}

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
			if (cmp(getKey(*j), getKey(_val)))
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
		rb_node* copyTree(rb_node* src)
		{
			if (!src)
				return nullptr;

			rb_node* root = new rb_node(*src);

			root->left = copyTree(src->left);
			if (root->left)
				root->left->parent = root;

			root->right = copyTree(src->right);
			if (root->right)
				root->right->parent = root;

			return root;
		}

		void makeEmpty(rb_node* x)
		{
			//不论是递归方法还是非递归方法，stack的使用都是必不可少的，既然如此，在空间效率上两者并无太大区别
			//虽然morris遍历只需要O(1)的空间复杂度，但是时间上却double了，因此不采用morris遍历
			while (x)
			{
				makeEmpty(x->right);
				auto y = x->left;
				delete x;
				x = y;
			}
		}

		iterator _insert(rb_node *x, rb_node *y, const ElemTypeDef& v)
		{
			rb_node *z = new rb_node(v, y);

			if (y == header || x || cmp(getKey(v), getKey(y->elem)))
			{
				y->left = z;
				if (y == header)
				{
					header->parent = z;
					header->right = z;
				}
				else if (y == header->left)
					header->left = z;
			}
			else
			{
				y->right = z;
				if (y == header->right)
					header->right = z;
			}

			_insert_rebalance(z, header->parent);
			++nodeCnt;

			return iterator(z, this);
		}

		void _insert_rebalance(rb_node* x, rb_node* &r)
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
				y->right->parent = x;
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
			rb_node *y = _erase_rebalance(pos.node, header->parent, header->left, header->right);
			delete y;
			--nodeCnt;
		}

		rb_node* _erase_rebalance(rb_node* z, rb_node* &r, rb_node* &lm, rb_node* &rm)
		{
			rb_node *y = z;
			rb_node *x = nullptr;
			rb_node *xp = nullptr;

			//x作为待删节点的孩子节点
			if (!y->left)
				x = y->right;
			else if (!y->right)
				x = y->left;
			else
			{
				y = y->right;
				while (y->left)
					y = y->left;
				x = y->right;
			}

			//re-link
			if (y != z)//z有两个孩子
			{
				//处理z的左子树
				z->left->parent = y;
				y->left = z->left;

				//处理z的右子树，有特殊情况，要分开考虑
				if (y == z->right)
					xp = y;
				else
				{
					xp = y->parent;
					if (x)
						x->parent = xp;
					y->parent->left = x;
					y->right = z->right;
					z->right->parent = y;
				}

				//z的parent的孩子指针的指向
				if (r == z)
					r = y;
				else if (z->parent->left == z)
					z->parent->left = y;
				else
					z->parent->right = y;
				y->parent = z->parent;

				//SGI的实现中不是交换节点中的元素，而是re-link替身节点和待删除节点，
				//然后swap两者的颜色，到达相同的效果
				std::swap(y->color, z->color);

				//让y指向待删除节点
				y = z;
			}
			else//z只有一个孩子或没有孩子
			{
				xp = y->parent;
				if (x)
					x->parent = xp;

				if (r == z)
					r = x;
				else if (z->parent->left == z)
					z->parent->left = x;
				else
					z->parent->right = x;

				if (lm == z)
					lm = z->right ? rb_node::_minimum(x) : z->parent;//由于z只有一个孩子或没有孩子，若z是leftmost，则没有左孩子，若此时z的右孩子为null，则z是叶子节点，leftmost即为z的parent，否则leftmost是x子树上的最左

				if (rm == z)
					rm = z->left ? rb_node::_maximum(x) : z->parent;//分析同上
			}

			//已经将待删节点y取下了，若是red可以直接return，否则要做调整后才能return
			//到这里所有情况都被简化成最基本的3种了：red leaf、black leaf 和 带一个child
			if (y->color != RED)
			{
				//到这步只可能是待删点是black leaf
				//若待删点是只有一个孩子，则孩子节点x必为red，直接pass 
				//若待删点是red leaf， 在上面的if就被pass了
				while (x != r && (!x || x->color == BLACK))//x是待调整节点
				{
					if (x == xp->left)
					{
						rb_node *w = xp->right;

						//若兄弟节点是red，则父节点必然是black，兄弟节点的两个子节点也是black，通过旋转与调色将其转换为下面的routine
						if (w->color == RED)
						{
							w->color = BLACK;
							xp->color = RED;
							_left_rotate(xp, r);
							w = xp->right;
						}

						//兄弟节点也是black，根据兄弟节点的孩子的情况分3种情况处理
						if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK))//无红节点
						{
							//更新待调整节点为父节点，继续调整
							//此处不更新xp为black:
							//若xp为red，则会跳出循环后被后面的if检测到，然后将xp的颜色改为black，树刚好平衡；
							//若xp为black，则还是在while循环中继续调整。
							w->color = RED;
							x = xp;
							xp = xp->parent;
						}
						else
						{
							if (!w->right || w->right->color == BLACK)//只有一个内侧红节点
							{
								if (w->left)//这个if是否多余？
									w->left->color = BLACK;
								w->color = RED;
								_right_rotate(w, r);
								w = xp->right;
							}

							//共同处理两个红孩子和一个外侧红孩子
							w->color = xp->color;
							xp->color = BLACK;
							if (w->right)//这个if是否多余？
								w->right->color = BLACK;
							_left_rotate(xp, r);
							break;
						}
					}
					else
					{
						rb_node *w = xp->left;
						if (w->color == RED)
						{
							w->color = BLACK;
							xp->color = RED;
							_right_rotate(xp, r);
							w = xp->left;
						}

						if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK))
						{
							w->color = RED;
							x = xp;
							xp = xp->parent;
						}
						else
						{
							if (!w->left || w->left->color == BLACK)
							{
								if (w->right)
									w->right->color = BLACK;
								w->color = RED;
								_left_rotate(w, r);
								w = xp->left;
							}

							w->color = xp->color;
							xp->color = BLACK;
							if (w->left)
								w->left->color = BLACK;
							_right_rotate(xp, r);
							break;
						}
					}
				}
				if (x)
					x->color = BLACK;
			}

			return y;
		}
	};

	template<typename KeyTypeDef, typename ValTypeDef, class Compare = std::less<KeyTypeDef>>
	class map
	{
	public:
		typedef pair<const KeyTypeDef, ValTypeDef> value_type;
	
	private:
		class _getKey
		{
		public:
			const KeyTypeDef& operator() (const value_type& _elem) const
			{
				return _elem.first; 
			}
		};

	public:
		typedef rb_tree<KeyTypeDef, value_type, _getKey, Compare> BalanceTreeTypeDef;

		typedef typename BalanceTreeTypeDef::iterator iterator;
		typedef typename BalanceTreeTypeDef::const_iterator const_iterator;

	private:
		BalanceTreeTypeDef *bt;

		void exchange(map &rhs)
		{
			std::swap(bt, rhs.bt);
		}

	public:
		map() :bt((BalanceTreeTypeDef *)std::malloc(sizeof(BalanceTreeTypeDef)))
		{
			new (bt) BalanceTreeTypeDef();
		}

		map(const map &other) :bt((BalanceTreeTypeDef *)std::malloc(sizeof(BalanceTreeTypeDef)))
		{
			new (bt) BalanceTreeTypeDef(*other.bt);
		}

		map& operator=(map tmp)
		{
			exchange(tmp);
			return *this;
		}

		~map()
		{
			bt->~BalanceTreeTypeDef();
			std::free(bt);
			bt = nullptr;
		}

		ValTypeDef& at(const KeyTypeDef &key)
		{
			iterator target = find(key);
			if (target == end())
				throw index_out_of_bound();

			return target->second;
		}

		const ValTypeDef& at(const KeyTypeDef &key) const
		{
			const_iterator target = find(key);
			if (target == cend())
				throw index_out_of_bound();

			return target->second;
		}

		ValTypeDef& operator[](const KeyTypeDef &key)
		{
			iterator target = find(key);
			if (target == end())
				return insert(value_type(key, ValTypeDef())).first->second;

			return target->second;
		}

		const ValTypeDef& operator[](const KeyTypeDef &key) const
		{
			const_iterator target = find(key);
			if (target == cend())
				throw index_out_of_bound();
			
			return target->second;
		}

		iterator begin() 
		{
			return bt->begin();
		}
        
		const_iterator cbegin() const 
		{
			return bt->cbegin();
		}

		iterator end()
		{
			return bt->end();
		}
        
		const_iterator cend() const
		{ 
			return bt->cend();
		}

		bool empty() const
		{
			return bt->empty();
		}

		size_t size() const
		{ 
			return bt->size();
		}

		void clear()
		{
			bt->clear();
		}

		size_t count(const KeyTypeDef &key) const
		{
			return find(key) != cend() ? 1 : 0; 
		}

		iterator find(const KeyTypeDef& _key)
		{
			return bt->find(_key);
		}
        
		const_iterator find(const KeyTypeDef& _key) const
		{
			return bt->find(_key);
		}

		pair<iterator, bool> insert(const value_type &_val) 
		{
			return bt->insert(_val);
		}

		void erase(iterator pos)
		{
			bt->erase(pos);
		}
	};
}
#endif

