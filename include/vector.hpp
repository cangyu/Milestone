#ifndef SJTU_VECTOR_HPP
#define SJTU_VECTOR_HPP

#include "exceptions.hpp"

#include <climits>
#include <cstddef>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <new>

namespace sjtu
{
	//a data container like std::vector
	//store data in a successive memory and support random access.
	template<typename T>
	class vector
	{
	private:
		T *start, *end_of_storage;
		size_t validLen;

		//operator=辅助函数
		void exchange(vector &rhs)
		{
			std::swap(start, rhs.start);
			std::swap(end_of_storage, rhs.end_of_storage);
			std::swap(validLen, rhs.validLen);
		}

		//将原空间扩大一倍并拷贝原始数据
		void doubleSpace()
		{
			if (capacity() == 0)
			{
				start = (T*)std::malloc(2 * sizeof(T));
				end_of_storage = start + 2;
			}
			else
			{
				size_t newSize = 2 * capacity();

				T *data = (T*)std::malloc(newSize * sizeof(T));
				for (auto i = 0; i < validLen; i++)
				{
					new (data + i) T(*(start + i));
					(start + i)->~T();
				}

				std::free(start);

				start = data;
				end_of_storage = start + newSize;
			}
		}

	public:
		class const_iterator;
		class iterator
		{
			friend class const_iterator;
			friend class vector<T>;

		private:
			vector<T> *ascription;
			T *start;
			int index;

		public:
			iterator(vector<T> *_a = nullptr, T *_s = nullptr, int _i = 0) :
				ascription(_a),
				start(_s),
				index(_i)
			{}

			iterator(const iterator &rhs) :
				ascription(rhs.ascription),
				start(rhs.start),
				index(rhs.index)
			{}

			~iterator() = default;

			//return a new iterator which points to the  n-next element,
			//even if there are not enough elements.
			iterator operator+(const int &n) const
			{
				iterator tmp(*this);
				tmp += n;
				return tmp;
			}

			iterator operator-(const int &n) const
			{
				iterator tmp(*this);
				tmp -= n;
				return tmp;
			}

			//return the distance between two iterator,
			//if these two iterators points to different vectors, throw invaild_iterator()
			int operator-(const iterator &rhs) const
			{
				if (ascription != rhs.ascription || start != rhs.start)
					throw invalid_iterator();

				return index - rhs.index;
			}

			iterator operator+=(const int &n)
			{
				index += n;
				return *this;
			}

			iterator operator-=(const int &n)
			{
				operator+=(-n);
				return *this;
			}

			iterator operator++(int)//后++不能作为左值
			{
				iterator tmp(*this);
				++*this;
				return tmp;
			}

			iterator& operator++()//前++可以作为左值，因此返回的是引用
			{
				operator+=(1);
				return *this;
			}

			iterator operator--(int)
			{
				iterator tmp(*this);
				--*this;
				return tmp;
			}

			iterator& operator--()
			{
				operator-=(1);
				return *this;
			}

			T& operator*() const
			{
				return *(start + index);
			}

			//check whether two iterators are same (pointing to the same memory).
			bool operator==(const iterator &rhs) const
			{
				if (ascription != rhs.ascription)
					return false;
				else if (start != rhs.start)
					return false;
				else
					return index == rhs.index;
			}

			bool operator==(const const_iterator &rhs) const
			{
				if (ascription != rhs.ascription)
					return false;
				else if (start != rhs.start)
					return false;
				else
					return index == rhs.index;
			}

			bool operator!=(const iterator &rhs) const
			{
				return !operator==(rhs);
			}

			bool operator!=(const const_iterator &rhs) const
			{
				return !operator==(rhs);
			}

			bool isValid(void *id) const
			{
				if (id && ascription != id)
					return false;
				else if (start != ((vector<T>*)id)->start)
					return false;
				else if (index < 0)
					return false;
				else
					return index <= ((vector<T>*)id)->validLen;
			}
		};

		//has same function as iterator, just for a const object.
		class const_iterator
		{
			friend class iterator;
			friend class vector<T>;

		private:
			const vector<T> *ascription;
			T *start;
			int index;

		public:
			const_iterator(const vector<T> *_a = nullptr, T *_s = nullptr, int _i = 0) :
				ascription(_a),
				start(_s),
				index(_i)
			{}

			const_iterator(const const_iterator &rhs) :
				ascription(rhs.ascription),
				start(rhs.start),
				index(rhs.index)
			{}

			//construct from iterator
			const_iterator(const iterator &rhs) :
				ascription(rhs.ascription),
				start(rhs.start),
				index(rhs.index)
			{}

			~const_iterator() = default;

			//return a new iterator which points to the  n-next element,
			//even if there are not enough elements.
			const_iterator operator+(const int &n) const
			{
				const_iterator tmp(*this);
				tmp += n;
				return tmp;
			}

			const_iterator operator-(const int &n) const
			{
				const_iterator tmp(*this);
				tmp -= n;
				return tmp;
			}

			//return the distance between two iterator,
			//if these two iterators points to different vectors, throw invaild_iterator()
			int operator-(const const_iterator &rhs) const
			{
				if (ascription != rhs.ascription || start != rhs.start)
					throw invalid_iterator();

				return index - rhs.index;
			}

			const_iterator operator+=(const int &n)
			{
				index += n;
				return *this;
			}

			const_iterator operator-=(const int &n)
			{
				operator+=(-n);
				return *this;
			}

			const_iterator operator++(int)//后++不能作为左值
			{
				iterator tmp(*this);
				++*this;
				return tmp;
			}

			const_iterator& operator++()//前++可以作为左值，因此返回的是引用
			{
				operator+=(1);
				return *this;
			}

			const_iterator operator--(int)
			{
				iterator tmp(*this);
				--*this;
				return tmp;
			}

			const_iterator& operator--()
			{
				operator-=(1);
				return *this;
			}

			const T& operator*() const
			{
				return *(start + index);
			}

			//check whether two iterators are same (pointing to the same memory).
			bool operator==(const iterator &rhs) const
			{
				if (ascription != rhs.ascription)
					return false;
				else if (start != rhs.start)
					return false;
				else
					return index == rhs.index;
			}

			bool operator==(const const_iterator &rhs) const
			{
				if (ascription != rhs.ascription)
					return false;
				else if (start != rhs.start)
					return false;
				else
					return index == rhs.index;
			}

			bool operator!=(const iterator &rhs) const
			{
				return !operator==(rhs);
			}

			bool operator!=(const const_iterator &rhs) const
			{
				return !operator==(rhs);
			}

			bool isValid(void *id) const
			{
				if (id && ascription != id)
					return false;
				else if (start != ((vector<T>*)id)->start)
					return false;
				else if (index < 0)
					return false;
				else
					return index <= ((vector<T>*)id)->validLen;
			}
		};

	public:
		vector() :
			start(nullptr),
			end_of_storage(nullptr),
			validLen(0)
		{}

		vector(const vector &rhs) :
			start(nullptr),
			end_of_storage(nullptr),
			validLen(rhs.validLen)
		{
			if (validLen > 0)
			{
				start = (T*)std::malloc(validLen * sizeof(T));
				end_of_storage = start + validLen;

				for (auto i = 0; i < validLen; i++)//不能简单地memcpy，有可能在元素内部还有动态指针，用构造函数来保证“深拷贝”的可靠性！！！
					new (start + i) T(*(rhs.start + i));
			}
		}

		vector(const std::vector<T> &rhs) :
			start(nullptr),
			end_of_storage(nullptr),
			validLen(rhs.size())
		{
			if (validLen > 0)
			{
				start = (T*)malloc(validLen * sizeof(T));
				end_of_storage = start + validLen;
				for (auto i = 0; i < validLen; i++)
					new (start + i) T(rhs[i]);
			}
		}

		vector(int n, const T &elem) :
			start(nullptr),
			end_of_storage(nullptr),
			validLen(n)
		{
			if (validLen > 0)
			{
				start = (T*)std::malloc(validLen * sizeof(T));
				end_of_storage = start + validLen;
				for (auto i = 0; i < validLen; i++)
					new (start + i) T(elem);
			}
		}

		~vector()
		{
			clear();
			free(start);
		}

		vector &operator=(vector rhs)
		{
			exchange(rhs);
			return *this;
		}

		//assigns specified element with bounds checking
		//throw index_out_of_bound if pos is not in [0, size)
		T& at(const size_t &pos)
		{
			if (pos >= size())
				throw index_out_of_bound();

			return *(start + pos);
		}

		const T& at(const size_t &pos) const //用于const对象
		{
			if (pos >= size())
				throw index_out_of_bound();

			return *(start + pos);
		}

		//assigns specified element with bounds checking
		//throw index_out_of_bound if pos is not in [0, size)
		//!!! Pay attention:
		//   In STL this operator does not check the boundary but we do this job here.
		T& operator[](const size_t &pos)
		{
			return at(pos);
		}

		const T& operator[](const size_t &pos) const //用于const对象
		{
			return at(pos);
		}

		//access the first element
		const T& front() const
		{
			if (size() == 0)
				throw container_is_empty();

			return *start;
		}

		//access the last element
		const T & back() const
		{
			if (size() == 0)
				throw container_is_empty();

			return *(start + validLen - 1);
		}

		//return an iterator to the beginning
		//注意：如果新建一个vector后就取auto it=begin(),然后 *it=xxx,会WA，在g++上也是这样
		iterator begin()
		{
			return iterator(this, start, 0);
		}

		const_iterator cbegin() const
		{
			return const_iterator(this, start, 0);
		}

		//return an iterator to the end
		iterator end()
		{
			return iterator(this, start, validLen);
		}

		const_iterator cend() const
		{
			return const_iterator(this, start, validLen);
		}

		//check whether the container is empty
		bool empty() const
		{
			return size() == 0;
		}

		//return the number of elements
		size_t size() const
		{
			return validLen;
		}

		//the number of elements that can be held in currently allocated storage
		size_t capacity() const
		{
			return end_of_storage ? (size_t)(end_of_storage - start) : 0;
		}

		//clear the contents
		void clear()
		{
			for (auto i = 0; i < validLen; i++)
				(start + i)->~T();

			validLen = 0;
		}

		//在pos所指向的位置之前插入新的value
		//返回指向新插入元素的iterator，pos可以是end()
		iterator insert(iterator pos, const T &value)
		{
			if (!pos.isValid(this))//若pos不属于本vector或者pos失效了，则无法插入
				throw invalid_iterator();

			return insert(pos.index, value);
		}

		//在给定的index之前插入新元素value
		//index可以为size()，即在尾部插入
		iterator insert(int index, const T &value)
		{
			//若index超出size()则非法
			if (index < 0 || index > size())
				throw index_out_of_bound();

			//若空间已满，要扩容
			if (size() == capacity())
				doubleSpace();

			//[index,end_of_storage)向后move一格
			size_t numToMove = validLen - index;
			T *dstStart = start + validLen;
			T *srcStart = dstStart - 1;

			for (auto i = 0; i < numToMove; i++)
			{
				new (dstStart - i) T(*(srcStart - i));
				(dstStart - i)->~T();
			}

			//insert
			new (start + index) T(value);
			++validLen;

			return iterator(this, start, index);
		}

		//删除由pos指定的位置上的元素
		//pos不能为end()，在index版本的erase会通过检查下标来确保不是end()
		iterator erase(iterator pos)
		{
			if (!pos.isValid(this))
				throw invalid_iterator();

			return erase(pos.index);
		}

		//删除由index指定的元素
		iterator erase(int index)
		{
			//index必须是有效的下标,包含了vector为空的情况
			if (index < 0 || index >= size())
				throw index_out_of_bound();

			//[index+1,end_of_storage)向前move一格
			size_t numToMove = size() - index - 1;
			T *dstStart = start + index;
			T *srcStart = dstStart + 1;

			for (auto i = 0; i < numToMove; i++)
			{
				(dstStart + i)->~T();
				new (dstStart + i) T(*(srcStart + i));
			}
			//释放最后一格处元素所占有的动态资源
			(dstStart + numToMove)->~T();
			--validLen;

			//return
			return iterator(this, start, index);
		}

		void push_back(const T &value)
		{
			insert(end(), value);
		}

		void pop_back()
		{
			if (size() == 0)
				throw container_is_empty();

			erase(--end());
		}
	};

}

#endif
