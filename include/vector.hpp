#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <cstdlib>

namespace sjtu 
{
/**
 * a data container like std::vector
 * store data in a successive memory and support random access.
 */
template<typename T>
class vector 
{
public:
	/**
	* a type for actions of the elements of a vector, and you should write
	*   a class named const_iterator with same interfaces.
	*/
	class const_iterator;
	class iterator 
	{
	private:
		T *_head, *_tail;
		T *cur;

	public:
		
		iterator(T *begin = nullptr, T *end = nullptr, T *pos = nullptr) :_head(begin), _tail(end), cur(pos) {}
		iterator(const iterator &rhs) :_head(rhs._head), _tail(rhs._tail), cur(rhs.cur) {}

		T* getCurPtr() const { return cur; }

		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		iterator operator+(const int &n) const 
		{
			iterator it(*this);
			it.cur += n;
			return it;
		}
		iterator operator-(const int &n) const
		{
			iterator it(*this);
			it.cur -= n;
			return it;
		}

		/**
		 * return the distance between two iterator,
		 * if these two iterators points to different vectors, throw invaild_iterator.
		 */ 
		int operator-(const iterator &rhs) const
		{
			if (!(_head == rhs._head && _tail == rhs._tail))
				throw invalid_iterator();

			return cur - rhs.cur;
		}
		iterator operator+=(const int &n)
		{
			cur += n;
			return *this;
		}
		iterator operator-=(const int &n) 
		{
			cur -= n;
			return *this;
		}

		/**
		 * iter++
		 */
		iterator operator++(int)//后++不能作为左值
		{
			iterator tmp(*this);
			cur += 1;
			return tmp;
		}

		/**
		 * ++iter
		 */
		iterator& operator++()//前++可以作为左值，因此返回的是引用
		{
			cur += 1;
			return *this;
		}

		/**
		 * iter--
		 */
		iterator operator--(int) 
		{
			iterator tmp;
			cur -= 1;
			return tmp;
		}

		/**
		 * --iter
		 */
		iterator& operator--() 
		{
			cur -= 1;
			return *this;
		}

		/**
		 * *it
		 */
		T& operator*() const { return *cur; }

		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
		bool operator==(const const_iterator &rhs) const { return cur == rhs.getCurPtr(); }
		
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
		bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }
	};

	/**
	 * has same function as iterator, just for a const object.
	 */
	class const_iterator
	{
	private:
		T *_head, *_tail;
		T *cur;

	public:
		const_iterator(T *begin = nullptr, T *end = nullptr, T *pos = nullptr) :_head(begin), _tail(end), cur(pos) {}
		const_iterator(const iterator &rhs) :_head(rhs._head), _tail(rhs._tail), cur(rhs.cur) {}

		T* getCurPtr() const { return cur; }

		/**
		* return a new iterator which pointer n-next elements
		*   even if there are not enough elements, just return the answer.
		* as well as operator-
		*/
		const_iterator operator+(const int &n) const
		{
			const_iterator it(*this);
			it.cur += n;
			return it;
		}
		const_iterator operator-(const int &n) const
		{
			const_iterator it(*this);
			it.cur -= n;
			return it;
		}

		/**
		 * return the distance between two const_iterators,
		 * if these two const_iterators points to different vectors, throw invaild_iterator.
		 */
		int operator-(const const_iterator &rhs) const
		{
			if (!(_head == rhs._head && _tail == rhs.tail))
				throw invalid_iterator();

			return cur - rhs.cur;
		}
		const_iterator operator+=(const int &n) 
		{
			cur += n;
			return *this;
		}
		const_iterator operator-=(const int &n)
		{
			cur -= n;
			return *this;
		}

		/**
		* iter++
		*/
		const_iterator operator++(int) //后++不能作为左值
		{
			const_iterator tmp(*this);
			cur += 1;
			return tmp;
		}

		/**
		* ++iter
		*/
		const_iterator& operator++()
		{
			cur += 1;
			return *this;
		}

		/**
		* iter--
		*/
		const_iterator operator--(int)
		{
			const_iterator tmp;
			cur -= 1;
			return tmp;
		}

		/**
		* --iter
		*/
		const_iterator& operator--()
		{
			cur -= 1;
			return *this;
		}

		/**
		* *it，和普通的iterator的区别在于此，解引用后得到的值不能作为左值继续使用，因此不是返回引用
		*/
		T operator*() const { return *cur; }
		
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
		bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
		
		/**
		* some other operator for iterator.
		*/
		bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
		bool operator!=(const const_iterator &rhs) const { return cur != rhs.cur; }
	};

protected:
	T *start, *finish, *end_of_storage;

public:
	/**
	 * Constructs
	 * At least three: default constructor, copy constructor and a constructor for std::vector
	 */
	vector() :start(nullptr),finish(nullptr),end_of_storage(nullptr){}
	vector(const vector &other) 
	{
		const int c = other.capacity();
		const int n = other.size();

		start = (T*)malloc(c * sizeof(T));
		end_of_storage = start + c;
		memcpy(start, other.start, n * sizeof(T));
		finish = start + n;
	}
	vector(const std::vector<T> &other)
	{
		const int c = other.capacity();
		const int n = other.size();

		start = (T*)malloc(c * sizeof(T));
		end_of_storage = start + c;
		for (int i = 0; i < n; i++)
			*(start + i) = other[i];
		finish = start + n;
	}
	vector(const int n, const T &elem)
	{
		start = (T*)malloc(n * sizeof(T));
		finish = end_of_storage = start + n;
		std::fill_n(start, n, elem);
	}

	/**
	 * Destructor
	 */
	~vector() 
	{
		free(start);
		start = finish = end_of_storage = nullptr;
	}

	/**
	 * Assignment operator
	 */
	vector &operator=(const vector &other) 
	{
		const int n = other.size();
		free(start);
		start = (T*)malloc(n * sizeof(T));
		memcpy(start, other.start, n * sizeof(T));
		finish = end_of_storage = start + n;
		return *this;
	}

	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 */
	T & at(const size_t &pos) 
	{
		if (!_bound_check(pos))
			throw index_out_of_bound();

		return *(start + pos); 
	}
	const T & at(const size_t &pos) const //用于const对象
	{
		if (!_bound_check(pos))
			throw index_out_of_bound();

		return *(start + pos);
	}

	/**
	 * assigns specified element with bounds checking
	 * throw index_out_of_bound if pos is not in [0, size)
	 * !!! Pay attentions
	 *   In STL this operator does not check the boundary but I want you to do.
	 */
	T & operator[](const size_t &pos) 
	{
		if (!_bound_check(pos))
			throw index_out_of_bound();

		return *(start + pos);
	}
	const T & operator[](const size_t &pos) const //用于const对象
	{
		if (!_bound_check(pos))
			throw index_out_of_bound();

		return *(start + pos);
	}

	/**
	 * access the first element.
	 * throw container_is_empty if size == 0
	 */
	const T & front() const 
	{
		if (size() == 0)
			throw container_is_empty();

		return *start;
	}
	/**
	 * access the last element.
	 * throw container_is_empty if size == 0
	 */
	const T & back() const 
	{
		if (size() == 0)
			throw container_is_empty();

		return *(finish - 1);
	}

	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() { return iterator(start, finish, start); }
	const_iterator cbegin() const { return const_iterator(start, finish, start); }
	/**
	 * returns an iterator to the end.
	 */
	iterator end() { return iterator(start, finish, finish); }
	const_iterator cend() const { return const_iterator(start, finish, finish); }
	
	/**
	 * checks whether the container is empty
	 */
	bool empty() const { return finish == start; }
	/**
	 * returns the number of elements
	 */
	size_t size() const { return (size_t)(finish - start); }
	/**
	 * returns the number of elements that can be held in currently allocated storage.
	 */
	size_t capacity() const { return (size_t)(end_of_storage - start); }
	/**
	 * clears the contents
	 */
	void clear() { finish = start; }

	/**
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value.
	 */
	iterator insert(iterator pos, const T &value) 
	{
		int targetIndex = pos - begin();

		if (!_bound_check(targetIndex))
			throw invalid_iterator();

		if (size() == capacity())
			doubleSpace();//有可能导致之前的迭代器失效,所以下面不能直接返回pos

		T *targetPtr = start + targetIndex;
		int num = finish - targetPtr;

		memmove(targetPtr + 1, targetPtr, num * sizeof(T));
		++finish;

		*targetPtr = value;

		return iterator(start, finish, targetPtr);
	}
	/**
	 * inserts value at index ind.
	 * after inserting, this->at(ind) == value is true
	 * returns an iterator pointing to the inserted value.
	 * throw index_out_of_bound if ind > size (in this situation ind can be size because after inserting the size will increase 1.)
	 */
	iterator insert(const size_t &ind, const T &value) 
	{
		if (!_bound_check(ind))
			throw index_out_of_bound();

		if (size() == capacity())
			doubleSpace();

		++finish;
		T *last = finish, *target = start + ind;
		while (last != target)//这样逐个操作与memmove相比效果如何？--不是一般的慢。。。
		{
			new (last) T(*(last - 1));
			--last;
		}

		new (last) T(value);

		return iterator(start, finish, last);
	}

	/**
	 * removes the element at pos.
	 * return an iterator pointing to the following element.
	 * If the iterator pos refers the last element, the end() iterator is returned.
	 */
	iterator erase(iterator pos) 
	{
		if (pos + 1 != end())
		{
			T *targetPtr = pos.getCurPtr();
			int num = finish - (targetPtr + 1);
			memmove(targetPtr, targetPtr + 1, num * sizeof(T));//memmove可保证在有局部重叠时结果的正确性，而memcpy则不能保证
		}
		--finish;
		finish->~T();
		return pos;
	}
	/**
	 * removes the element with index ind.
	 * return an iterator pointing to the following element.
	 * throw index_out_of_bound if ind >= size
	 */
	iterator erase(const size_t &ind)
	{
		if (ind >= size())
			throw index_out_of_bound();

		if (ind + 1 != size())
		{
			T *targetPtr = start + ind;
			int num = finish - (targetPtr + 1);
			memmove(targetPtr, targetPtr + 1, num * sizeof(T));//注意这里传过去的数量是以字节为单位的！！！
		}
		--finish;
		finish->~T();
		return iterator(start, finish, start + ind);
	}
	/**
	 * adds an element to the end.
	 */
	void push_back(const T &value) 
	{
		if (size() == capacity())
			doubleSpace();

		new (finish) T(value);
		++finish;
	}
	/**
	 * remove the last element from the end.
	 * throw container_is_empty if size() == 0
	 */
	void pop_back() 
	{
		if (size() == 0)
			throw container_is_empty();

		--finish;
		finish->~T();
	}

private:
	bool _bound_check(const size_t &pos) const { return pos >= 0 && pos < size(); }

	void doubleSpace()
	{
		if (capacity() == 0)
		{
			start = (T*)malloc(2 * sizeof(T));
			finish = start;
			end_of_storage = start + 2;
		}
		else
		{
			int originalSize = capacity();
			T *data = (T*)malloc(2 * originalSize * sizeof(T));
			memcpy(data, start, originalSize * sizeof(T));
			free(start);
			start = data;
			finish = start + originalSize;
			end_of_storage = start + originalSize * 2;
		}
	}
};

}

#endif
