#ifndef SJTU_UNORDERED_MAP_HPP
#define SJTU_UNORDERED_MAP_HPP

#include <functional>
#include <cstddef>
#include "exceptions.hpp"
#include "utility.hpp"

namespace sjtu {

template<
	class Key, 
	class T,
	class Hash = std::hash<Key>,
	class KeyEqual = std::equal_to<Key>
> class unordered_map {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 */
	typedef pair<const Key, T> value_type;
	/**
	 * see BidirectionalIterator at CppReference for help.
	 */
	class const_iterator;
	class iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
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
		iterator& operator++() {}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type& operator*() const {}
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
	/**
	 * iterators for a certain hash value elements, behave like list::iterator
	 */
	class local_iterator {
	private:
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO iter++
		 */
		local_iterator operator++(int) {}
		/**
		 * TODO ++iter
		 */
		local_iterator& operator++() {}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type& operator*() const{}
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
	class const_local_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			// data members.
		public:
			const_local_iterator() {
				// TODO
			}
			const_local_iterator(const const_local_iterator &other) {
				// TODO
			}
			const_local_iterator(const iterator &other) {
				// TODO
			}
			// And other methods in iterator.
			// And other methods in iterator.
			// And other methods in iterator.
	};
	/**
	 * TODO Constructor
	 */
	unordered_map() {}
	unordered_map(const unordered_map &other) {}
	/**
	 * TODO Destructor
	 */
	~unordered_map() {}
	/**
	 * TODO assignment operator
	 */
	unordered_map &operator=(const unordered_map &other) {}
	/**
	 * returns an iterator to the beginning
	 */
	iterator begin() {}
	const_iterator cbegin() {}
	/**
	 * returns an iterator to the end
	 */
	iterator end() {}
	const_iterator cend() {}
	/**
	 * checks whether the conatiner is empty.
	 */
	bool empty() const {}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {}
	/**
	 * clears the contents.
	 */
	void clear() {}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion), 
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {}
	/**
	 * erase the element at pos.
	 */
	void erase(iterator pos) {}
	/**
	 * Returns the number of elements with key 
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0 
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {}
	const_iterator find(const Key &key) const {}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {}
	const T & at(const Key &key) const {}
	/**
	 * TODO
	 * access specified element 
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {}
	const T & operator[](const Key &key) const {}
	/**
	 * bucket interfaces
	 */
	/**
	 * returns an iterator to the beginning of the specified bucket.
	 */
	local_iterator begin(const size_t &n) {}
	const_local_iterator cbegin(const size_t &n) const {}
	/**
	 * returns an iterator to the end of the specified bucket.
	 */
	local_iterator end(const size_t &n) {}
	const_local_iterator cend(const size_t &n) const {}
	/**
	 * returns the number of buckets.
	 */
	size_t bucket_count() const {}
	/**
	 * returns the number of elements in specific bucket.
	 */
	size_t bucket_size(const size_t &n) const {}
	/**
	 * returns average number of elements per buckets.
	 */
	double load_factor() const {}
};
}

#endif