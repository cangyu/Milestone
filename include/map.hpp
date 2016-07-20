#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu 
{
	//A container like std::map.
	template<typename KeyTypeDef, typename ValTypeDef, class Compare = std::less<KeyTypeDef>>
	class map
	{
	private:
		//Internal type used in selected balance tree.
		typedef pair<const KeyTypeDef, ValTypeDef> ElemTypeDef;

		//Functor used in selected balance tree.
		class _cmp_funcor
		{
		private:
			Compare cmp;

		public:
			_cmp_funcor() = default;
			~_cmp_funcor() = default;
			bool operator()(const ElemTypeDef &lhs, const ElemTypeDef &rhs) const 
			{
				return cmp(lhs.first, rhs.first);
			}
		};

		//Type of the balance tree inside a map.
		typedef TreeTypeDef rb_tree<ElemTypeDef, _cmp_funcor>;

		//Internal tree
		TreeTypeDef *t;

		//helper function for operation=
		void exchange(map &rhs)
		{
			std::swap(t, rhs.t);
		}

	protected:
		//Iterators
		typedef typename TreeTypeDef::iterator iterator;
		typedef typename TreeTypeDef::const_iterator const_iterator;

	public:
		//Constructors
		map() {}
		map(const map &rhs) 
		{

		}

		//Assignment operator
		map& operator=(map rhs)
		{
			exchange(rhs);
			return *this;
		}

		//Destructor
		~map() 
		{
			delete t;
			t = nullptr;
		}

		//Member function at
		ValTypeDef &at(const KeyTypeDef &key) 
		{

		}
		const ValTypeDef &at(const KeyTypeDef &key) const
		{

		}

		//operator[]
		ValTypeDef& operator[](const KeyTypeDef &key)
		{

		}
		const ValTypeDef& operator[](const KeyTypeDef &key) const
		{

		}


		//get starting position
		iterator begin()
		{

		}
		const_iterator cbegin() const
		{

		}

		//get ending position, next to last element.
		iterator end()
		{

		}
		const_iterator cend() const
		{

		}

		//checks whether the container is empty
		bool empty() const
		{

		}

		//returns the number of elements.
		size_t size() const 
		{

		}

		//clears the contents
		void clear()
		{

		}

		/**
		* insert an element.
		* return a pair, the first of the pair is
		*   the iterator to the new element (or the element that prevented the insertion),
		*   the second one is true if insert successfully, or false.
		*/
		pair<iterator, bool> insert(const ElemTypeDef &elem)
		{

		}

		/**
		* erase the element at pos.
		* throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
		*/
		void erase(iterator pos) 
		{

		}

		/**
		* Returns the number of elements whose key compares equivalent to the specified argument.
		* The answer is either 1 or 0, since this container does not allow duplicates.
		* The default method of check the equivalence is !(a < b || b > a)
		*/
		size_t count(const KeyTypeDef &key) const 
		{

		}

		/**
		* Finds an element with key equivalent to key.
		* key value of the element to search for.
		* Iterator to an element with key equivalent to key.
		* If no such element is found, past-the-end (see end()) iterator is returned.
		*/
		iterator find(const KeyTypeDef &key)
		{

		}
		const_iterator find(const KeyTypeDef &key) const
		{

		}
	};


	//Red-Black Tree for implementation of map.
	template<typename ElemTypeDef, class Compare>
	class rb_tree
	{
	private:
		typedef enum { RED = 0, BLACK = 1 } ColorTypeDef;

		class iterator
		{

		};

		class const_iterator
		{

		};


	};
}

#endif