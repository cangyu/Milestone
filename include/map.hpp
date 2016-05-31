#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

#include <cstdlib>
#include <ctime>

namespace sjtu 
{
/**
 * a container like std::map
 */
template<class Key, class T, class Compare = std::less<Key> >
class map
{
public:
	typedef pair<const Key, T> value_type;
	typedef struct node
	{
		node *left, *right, *parent;
		value_type *data;

		node() :left(nullptr), right(nullptr), parent(nullptr), data(nullptr) {}
		node(const value_type &_d, node *_p = nullptr, node *_l = nullptr, node *_r = nullptr) :left(_l), right(_r), parent(_p), data(new value_type(_d)) {}
		node(const node &other) :left(other.left), right(other.right), parent(other.parent), data(nullptr)
		{
			if (!other.data)
				data = new T(*other.data);
		}
		node &operator=(const node &rhs)
		{
			node tmp(rhs);
			std::swap(left, tmp.left);
			std::swap(right, tmp.right);
			std::swap(parent, tmp.parent);
			std::swap(data, tmp.data);
			return *this;
		}
		~node()
		{
			if (!data)
				delete data;
			data = nullptr;
		}
	}NodeTypeDef, *pNodeTypeDef;

private:
	pNodeTypeDef root, header;
	size_t nodeCnt;
	Compare *cmp;//设成指针形式便于swap

	void swap(map &rhs)
	{
		using std::swap;
		swap(root, rhs.root);
		swap(header, rhs.header);
		swap(nodeCnt, rhs.nodeCnt);
		swap(cmp, rhs.cmp);
	}
	void makeEmpty(pNodeTypeDef &_r)
	{
		if (!_r)
			return;

		makeEmpty(_r->left);
		makeEmpty(_r->right);
		delete _r;
	}
	bool checkAscription(const iterator &it) const	{ return it.identity() == header; }
	bool checkAscription(const const_iterator &it) const { return it.identity() == header; }
	void updateLeftMost()
	{
		pNodeTypeDef tmp = header->left;
		while (tmp->left)
			tmp = tmp->left;
		header->left = left;
	}
	void updateRightMost()
	{
		pNodeTypeDef tmp = header->right;
		while (tmp->right)
			tmp = tmp->right;
		header->right = tmp;
	}

	pair<pNodeTypeDef,bool> insert(const value_type &val, pNodeTypeDef &r, pNodeTypeDef p)
	{
		if (!r)
		{
			r = new NodeTypeDef(val, p);
			++nodeCnt;
			updateLeftMost();
			updateRightMost();
			return pair<pNodeTypeDef, bool>(r, true);
		}
		else if ((*cmp)(val.first, r->data->first))
			return insert(val, r->left, r);
		else if ((*cmp)(r->data->first, val.first))
			return insert(val, r->right, r);
		else
			return pair<pNodeTypeDef, bool>(r, false);
	}
	pNodeTypeDef find(pNodeTypeDef r, const Key &k)
	{
		if (!r)
			return header;
		else if ((*cmp)(k, r->data->first))
			return find(r->left, k);
		else if ((*cmp)(r->data->first, k))
			return find(r->right, k);
		else
			return r;
	}
	
public:
	//def of iterator
	class const_iterator;
	class iterator
	{
	private:
		pNodeTypeDef cur, bound;

		void increasePtr()
		{
			if (cur->right)
			{
				pNodeTypeDef tmp = cur->right;
				while (tmp->left)
					tmp = tmp->left;
				cur = tmp;
			}
			else
			{
				pNodeTypeDef p = cur->parent;
				while (p && p->right == cur)
				{
					cur = p;
					p = p->parent;
				}
				if (p)
					cur = p;
			}
		}

		void decreasePtr()
		{
			if (cur == header)//最后一个
				cur = header->right;
			else//不是最后一个，按常规方法找前驱
			{
				if (cur->left)
				{
					pNodeTypeDef tmp = cur->left;
					while (tmp->right)
						tmp = tmp->right;
					cur = tmp;
				}
				else
				{
					pNodeTypeDef p = cur->parent;
					while (p && p->left == cur)
					{
						cur = p;
						p = p->parent;
					}
					if (p)
						cur = p;
				}
			}
		}

	public:
		iterator() :cur(nullptr),bound(header)	{}
		iterator(const iterator &other) :cur(other.cur), bound(other.bound)	{}

		/**
		 * iter++
		 */
		iterator operator++(int) 
		{
			iterator tmp(*this);
			increasePtr();
			return tmp;
		}
		/**
		 *  ++iter
		 */
		iterator & operator++() 
		{
			increasePtr();
			return *this;
		}

		/**
		 * iter--
		 */
		iterator operator--(int) 
		{
			iterator tmp(*this);
			decreasePtr();
			return tmp;
		}
		/**
		 * --iter
		 */
		iterator & operator--() 
		{
			decreasePtr();
			return *this;
		}

		/**
		 *  *iter
		 */
		value_type& operator*() const { return *cur->data; }

		/**
		 * an operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
		bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
		bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }

		/**
		 * for the support of it->first. 
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept{ return cur; }
	};
	class const_iterator 
	{
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			pNodeTypeDef cur, bound;

			void increasePtr()
			{
				if (cur->right)
				{
					pNodeTypeDef tmp = cur->right;
					while (tmp->left)
						tmp = tmp->left;
					cur = tmp;
				}
				else
				{
					pNodeTypeDef p = cur->parent;
					while (p && p->right == cur)
					{
						cur = p;
						p = p->parent;
					}
					if (p)
						cur = p;
				}
			}

			void decreasePtr()
			{
				if (cur == header)//最后一个
					cur = header->right;
				else//不是最后一个，按常规方法找前驱
				{
					if (cur->left)
					{
						pNodeTypeDef tmp = cur->left;
						while (tmp->right)
							tmp = tmp->right;
						cur = tmp;
					}
					else
					{
						pNodeTypeDef p = cur->parent;
						while (p && p->left == cur)
						{
							cur = p;
							p = p->parent;
						}
						if (p)
							cur = p;
					}
				}
			}
		public:
			const_iterator() :cur(nullptr), bound(header)	{}
			const_iterator(const const_iterator &other) :cur(other.cur), bound(other.bound)	{}
			const_iterator(const iterator &other) :cur(other.cur), bound(other.bound)	{}

			/**
			* iter++
			*/
			const_iterator operator++(int)
			{
				const_iterator tmp(*this);
				increasePtr();
				return tmp;
			}
			/**
			*  ++iter
			*/
			const_iterator & operator++()
			{
				increasePtr();
				return *this;
			}

			/**
			* iter--
			*/
			const_iterator operator--(int)
			{
				const_iterator tmp(*this);
				decreasePtr();
				return tmp;
			}
			/**
			* --iter
			*/
			const_iterator & operator--()
			{
				decreasePtr();
				return *this;
			}

			/**
			*  *iter
			*/
			value_type operator*() const { return *cur->data; }

			/**
			* an operator to check whether two iterators are same (pointing to the same memory).
			*/
			bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
			bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
			/**
			* some other operator for iterator.
			*/
			bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
			bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }

			/**
			* for the support of it->first.
			* See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
			*/
			value_type* operator->() const noexcept{ return cur; }
	};

	/**
	 * constructors
	 */
	map() :root(nullptr), header(new NodeTypeDef()), nodeCnt(0), cmp(new Compare()) {}
	map(const map &other) :root(nullptr), header(new NodeTypeDef()), nodeCnt(0), cmp(new Compare(other.cmp))
	{
		for (auto it = other.cbegin(); it != other.cend(); ++it)
			insert(*it);
	}
	
	/**
	 * assignment operator
	 */
	map & operator=(const map &other) 
	{
		map tmp(other);
		swap(rmp);
		return *this;
	}

	/**
	 * Destructors
	 */
	~map() 
	{
		makeEmpty(root);
		delete header;
		delete cmp;
	}

	/**
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) 
	{
		iterator target = find(key);
		if (target == end())
			throw index_out_of_bound();

		return target->second;
	}
	const T & at(const Key &key) const 
	{
		const_iterator target = find(key);
		if (target == cend())
			throw index_out_of_bound();

		return target->second;
	}

	/**
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 * performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) 
	{
		iterator target = find(key);
		if (target != end())
			return target->second;
		else
			return insert(pair<Key, T>(key, T()))->second;
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const 
	{
		const_iterator = find(key);
		if (target != cend())
			return target->second;
		else
			throw index_out_of_bound();
	}

	/**
	 * return an iterator to the beginning
	 */
	iterator begin() { return iterator(header->left); }
	const_iterator cbegin() const { return const_iterator(header->left); }

	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() { return iterator(header); }
	const_iterator cend() const { return const_iterator(header); }

	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const { return header->left == header && header->right == header; }
	
	/**
	 * returns the number of elements.
	 */
	size_t size() const { return nodeCnt; }

	/**
	 * clears the contents
	 */
	void clear() 
	{
		makeEmpty(root);
		root = nullptr;
		header->left = header->right = nullptr;
		nodeCnt = 0;
	}

	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 * the iterator to the new element (or the element that prevented the insertion), 
	 * the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) 
	{
		pair<pNodeTypeDef, bool> ans = insert(value, root, header);
		return pair<iterator, bool>(iterator(ans.first), ans.second);
	}

	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) 
	{
		if (pos == end() || !checkAscription(pos))
			throw invalid_iterator();

		if (pos->left && pos->right)//有两个儿子
		{
			std::srand(std::time(nullptr));//找替身的方法，为了尽量保持平衡，采用随机地方式选取替身节点
			bool chooseRight = ((rand() % 32767) / 32767.0) > 0.5;
			if (chooseRight)
			{
				pNodeTypeDef tmp = t->right;
				while (tmp->left)
					tmp = tmp->left;
				
				tmp->parent->left = nullptr;
				if (pos == pos->parent->left)
					pos->parent->left = tmp;
				else
					pos->parent->right = tmp;

				tmp->left = pos->left;
				tmp->right = pos->right;
				tmp->parent = pos->parent;

				delete pos;
			}
			else
			{
				pNodeTypeDef tmp = t->left;
				while (tmp->right)
					tmp = tmp->right;

				tmp->parent->right = nullptr;
				if (pos == pos->parent->left)
					pos->parent->left = tmp;
				else
					pos->parent->right = tmp;

				tmp->left = pos->left;
				tmp->right = pos->right;
				tmp->parent = pos->parent;

				delete pos;
			}
		}
		else if (!pos->left && !pos->right)//没有儿子
		{
			if (pos == pos->parent->left)
				pos->parent->left = nullptr;
			else
				pos->parent->right = nullptr;

			delete pos;
		}
		else//有一个儿子
		{
			pNodeTypeDef tmp = pos->left ? pos->left : pos->right;
			tmp->parent = pos->parent;
			if (pos == pos->parent->left)
				pos->parent->left = tmp;
			else
				pos->parent->right = tmp;

			delete pos;
		}
	}

	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const { return find(key) != cend() ? 1 : 0; }

	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) 	{ return iterator(find(root, key)); }
	const_iterator find(const Key &key) const { return const_iterator(find(root, key)); }
};

}

#endif
