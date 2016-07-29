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

				//����Ƿ�ָ����end()
				if (ascription->header && ascription->header == node)
					return false;

				return true;
			}
			
		private:
			//��iteratorָ��������һ��Ԫ�أ�increment���������Ķ�����ڶ���Ԫ��
			//��iteratorָ����������һ��Ԫ�أ�increment����header
			//��iteratorָ��header��incerment�������һ��Ԫ�ص�����node�������Ϊ��δ�����
			void increment()
			{
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

					//Corner case��nodeԭ��ָ��root��rootû���Һ��ӣ����Ծ��������while��node��header��y��root����ʱ���ܽ�node��Ϊy
					if (node->right != y)
						node = y;
				}
			}

			//��iteratorָ��������һ��Ԫ�أ�decrement����header
			//��iteratorָ����������һ��Ԫ�أ�decrement����������ǰһ��node
			//��iteratorָ��header��decrement���ǵ�һ��node������
			void decrement()
			{
				if (node->color == RED && node->parent->parent == node)//node�п���ָ��header����Ϊheader��root�����о�������������parent��ָ���Լ������ʣ���Ҫ����ɫ���޶�ȷ����header
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

					node = y;//��nodeָ���һ���ڵ㣬��--��ָ��header
				}
			}
		};

		class const_iterator
		{
		private:
			rb_node* node;
			rb_tree* ascription;

		public:
			const_iterator(rb_node* _nPtr = nullptr, rb_tree* _tPtr = nullptr) :node(_nPtr), ascription(_tPtr) {}
			const_iterator(const const_iterator& rhs) :node(rhs.node), ascription(rhs.ascription) {}
			const_iterator(const iterator& rhs) :node(rhs.node), ascription(rhs.ascription) {}

			const ElemTypeDef& operator*() const { return node->elem; }
			const ElemTypeDef* operator->() const noexcept { return &(operator*()); }//���صõ���ָ����Ա��޸ģ�����ָ��ָ������ݲ��ܱ��޸�

			const_iterator operator++(int)
			{
				const_iterator tmp = *this;
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
				const_iterator tmp = *this;
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
				return id && id == ascription;
			}
			bool isValid() const
			{
				if (ascription == nullptr || node == nullptr)
					return false;

				//����Ƿ�ָ����end()
				if (ascription->header && ascription->header == node)
					return false;

				return true;
			}

		private:
			void increment()
			{
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

					//Corner case��nodeԭ��ָ��root��rootû���Һ��ӣ����Ծ��������while��node��header��y��root����ʱ���ܽ�node��Ϊy
					if (node->right != y)
						node = y;
				}
			}
			void decrement()
			{
				if (node->color == RED && node->parent->parent == node)//node�п���ָ��header����Ϊheader��root�����о�������������parent��ָ���Լ������ʣ���Ҫ����ɫ���޶�ȷ����header
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

					node = y;//��nodeָ���һ���ڵ㣬��--��ָ��header
				}
			}
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

			_insert_rebalance(z, header->parent);
			++nodeCnt;

			return iterator(z, this);
		}

		void _insert_rebalance(rb_node* x, rb_node* &r)
		{
			x->color = RED;
			while (x != r && x->parent->color == RED)
			{
				if (x->parent == x->parent->parent->left)//���ڵ���Ϊ����Ӵ���
				{
					rb_node *y = x->parent->parent->right;//x�Ĳ����ڵ�
					if (y && y->color == RED)//�����ڵ�Ҳ��RED��������ɫ�ټ������ϵ�������
					{
						x->parent->color = BLACK;
						y->color = BLACK;
						x->parent->parent->color = RED;
						x = x->parent->parent;//��������
					}
					else
					{
						if (x == x->parent->right)//LR������һ������ת��ת����LL
						{
							x = x->parent;
							_left_rotate(x, r);
						}
						x->parent->color = BLACK;
						x->parent->parent->color = RED;
						_right_rotate(x->parent->parent, r);
					}
				}
				else//���ڵ���Ϊ�Ҷ��Ӵ���
				{
					rb_node *y = x->parent->parent->left;//x�Ĳ����ڵ�
					if (y && y->color == RED)//ͬ��
					{
						x->parent->color = BLACK;
						y->color = BLACK;
						x->parent->parent->color = RED;
						x = x->parent->parent;
					}
					else
					{
						if (x == x->parent->left)//RL������һ������ת��ת����RR
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
			rb_node *y = _erase_rebalance(pos->node, header->parent, header->left, header->right);
			delete y;
			--nodeCnt;
		}

		rb_node* _erase_rebalance(rb_node* z, rb_node* &r, rb_node* &lm, rb_node* &rm)
		{
			rb_node *y = z;
			rb_node *x = nullptr;
			rb_node *xp = nullptr;

			//x��Ϊ��ɾ�ڵ�ĺ��ӽڵ�
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
			if (y != z)//z����������
			{
				//����z��������
				z->left->parent = y;
				y->left = z->left;

				//����z���������������������Ҫ�ֿ�����
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

				//z��parent�ĺ���ָ���ָ��
				if (r == z)
					r = y;
				else if (z->parent->left == z)
					z->parent->left = y;
				else
					z->parent->right = y;
				y->parent = z->parent;

				//SGI��ʵ���в��ǽ����ڵ��е�Ԫ�أ�����re-link�����ڵ�ʹ�ɾ���ڵ㣬
				//Ȼ��swap���ߵ���ɫ��������ͬ��Ч��
				std::swap(y->color, z->color);

				//��yָ���ɾ���ڵ�
				y = z;
			}
			else//zֻ��һ�����ӻ�û�к���
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
					lm = z->right ? _minimum(x) : z->parent;//����zֻ��һ�����ӻ�û�к��ӣ���z��leftmost����û�����ӣ�����ʱz���Һ���Ϊnull����z��Ҷ�ӽڵ㣬leftmost��Ϊz��parent������leftmost��x�����ϵ�����

				if (rm == z)
					rm = z->left ? _maximum(x) : z->parent;//����ͬ��
			}

			//�Ѿ�����ɾ�ڵ�yȡ���ˣ�����red����ֱ��return������Ҫ�����������return
			//������������������򻯳��������3���ˣ�red leaf��black leaf �� ��һ��child
			if (y->color != RED)
			{
				//���ⲽֻ�����Ǵ�ɾ����black leaf
				//����ɾ����ֻ��һ�����ӣ����ӽڵ�x��Ϊred��ֱ��pass 
				//����ɾ����red leaf�� �������if�ͱ�pass��
				while (x != r && (!x || x->color == BLACK)//x�Ǵ������ڵ�
				{
					if (x == xp->left)
					{
						rb_node *w = xp->right;

						//���ֵܽڵ���red���򸸽ڵ��Ȼ��black���ֵܽڵ�������ӽڵ�Ҳ��black��ͨ����ת���ɫ����ת��Ϊ�����routine
						if (w->color == RED)
						{
							w->color = BLACK;
							xp->color = RED;
							_left_rotate(xp, r);
							w = xp->right;
						}

						//�ֵܽڵ�Ҳ��black�������ֵܽڵ�ĺ��ӵ������3���������
						if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK))//�޺�ڵ�
						{
							//���´������ڵ�Ϊ���ڵ㣬��������
							//�˴�������xpΪblack:
							//��xpΪred���������ѭ���󱻺����if��⵽��Ȼ��xp����ɫ��Ϊblack�����պ�ƽ�⣻
							//��xpΪblack��������whileѭ���м���������
							w->color = RED;
							x = xp;
							xp = xp->parent;
						}
						else
						{
							if (!w->right || w->right->color == BLACK)//ֻ��һ���ڲ��ڵ�
							{
								if (w->left)//���if�Ƿ���ࣿ
									w->left->color = BLACK;
								w->color = RED;
								_right_rotate(w, r);
								w = xp->right;
							}

							//��ͬ���������캢�Ӻ�һ�����캢��
							w->color = xp->color;
							xp->color = BLACK;
							if (w->right)//���if�Ƿ���ࣿ
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
}

#endif