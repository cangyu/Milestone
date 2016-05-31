#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu 
{
/**
 * a container like std::map
 */
template<class Key, class T, class Compare = std::less<Key>, class BalanceTree = BST<Key, T, std::select1st<pair<Key, T> >, Compare> >
class map
{
private:
	Compare *cmp;
	BalanceTree *bt;

public:
	/**
	 * the internal type of data.
	 */
	typedef pair<const Key, T> value_type;

	typedef iterator BlanceTree::iterator;
	typedef const_iterator BalanceTree::const_iterator;

	/**
	 * two constructors
	 */
	map() :cmp(new Compare()), bt(new BalanceTree(*cmp)) {}
	map(const map &other) :cmp(new Compare(*other.cmp)), bt(new BalanceTree(*other.bt)) {}
	
	/**
	 * assignment operator
	 */
	map & operator=(const map &other) 
	{
		map tmp(other);
		std::swap(cmp, tmp.cmp);
		std::swap(bt, tmp.bt);
		return *this;
	}

	/**
	 * Destructors
	 */
	~map() 
	{
		delete cmp;
		delete bt;
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
	 *   performing an insertion if such key does not already exist.
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
	 * return a iterator to the beginning
	 */
	iterator begin() { return bt.begin(); }
	const_iterator cbegin() const { return bt.cbegin(); }

	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() { return bt.end(); }
	const_iterator cend() const { return bt.cend(); }

	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const { return bt.empty(); }

	/**
	 * returns the number of elements.
	 */
	size_t size() const { return bt.size(); }

	/**
	 * clears the contents
	 */
	void clear() { return bt.clear(); }

	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) { return bt.insert(value); }

	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) 
	{
		if (pos == end() || pos.identity() != bt.identity())
			throw invalid_iterator();

		bt.erase(pos);
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
	iterator find(const Key &key) { return bt.find(key); }
	const_iterator find(const Key &key) const { return bt.find(key); }
};

template<typename Key,typename T,class KeyOfElem, class Compare=std::less<Key> >
class BST
{
public:
	struct node
	{
		node* left, right;
		T *data;

		node() :left(nullptr), right(nullptr), data(nullptr) {}
		node(node *l, node *r, const T &d) :left(l), right(r), data(new T(d)) {}
		node(const node &rhs) :left(rhs.left), right(rhs.right), data(new T(*(rhs.data)){}
		~node() 
		{
			if (!data)
				delete data; 
			data = nullptr;
		}
	};

protected:
	const node *nil = node();
	node *leftmost;
	node *rightmost;
	node *root;

	size_t nodeCnt;
	Compare cmp;

public:
	BST() :leftmost(nil), rightmost(nil), root(nullptr), nodeCnt(0), cmp(Compare()) {}
	BST(const Compare &other):leftmost(nil), rightmost(nil), root(nullptr), nodeCnt(0), cmp(other) {}
	~BST() { clear(); }

	/**
	* see BidirectionalIterator at CppReference for help.
	*
	* if there is anything wrong throw invalid_iterator.
	*     like it = map.begin(); --it;
	*       or it = map.end(); ++end();
	*/
	class const_iterator;
	class iterator {
	private:
		/**
		* TODO add data members
		*   just add whatever you want.
		*/
	public:
		iterator() {
			// TODO
		}
		iterator(const iterator &other) {
			// TODO
		}
		/**
		* return a new iterator which pointer n-next elements
		*   even if there are not enough elements, just return the answer.
		* as well as operator-
		*/
		/**
		* TODO iter++
		*/
		iterator operator++(int) {}
		/**
		* TODO ++iter
		*/
		iterator & operator++() {}
		/**
		* TODO iter--
		*/
		iterator operator--(int) {}
		/**
		* TODO --iter
		*/
		iterator & operator--() {}
		/**
		* a operator to check whether two iterators are same (pointing to the same memory).
		*/
		value_type & operator*() const {}
		bool operator==(const iterator &rhs) const {}
		bool operator==(const const_iterator &rhs) const {}
		/**
		* some other operator for iterator.
		*/
		bool operator!=(const iterator &rhs) const {}
		bool operator!=(const const_iterator &rhs) const {}

		/**
		* for the support of it->first.
		* See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		*/
		value_type* operator->() const noexcept {}
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




};



}

#endif
