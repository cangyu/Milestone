#ifndef SJTU_UNORDERED_MAP_HPP
#define SJTU_UNORDERED_MAP_HPP

#include <utility>
#include <functional>
#include <cstddef>
#include "exceptions.hpp"
#include "utility.hpp"
#include "vector.hpp"

namespace sjtu
{
template<class Key, class T, class Hash = std::hash<Key>, class KeyEqual = std::equal_to<Key>> 
class unordered_map 
{
public:
	typedef pair<const Key, T> value_type;

private:
	struct node
	{
		value_type elem;
		node* next;

		node() :elem(),next(nullptr) {}
		node(const value_type& _val, node* _next = nullptr) :elem(_val), next(_next) {}
	};

	struct bucket
	{
		size_t cnt;
		node* start;

		bucket() :cnt(0), start(nullptr) {}
	};

	//hash表的大小取质数
	static const int _num_prime = 28;
	static const unsigned long _prime_list[_num_prime] ={
		53,         97,           193,         389,       769,
		1543,       3079,         6151,        12289,     24593,
		49157,      98317,        196613,      393241,    786433,
		1572869,    3145739,      6291469,     12582917,  25165843,
		50331653,   100663319,    201326611,   402653189, 805306457,
		1610612741, 3221225473ul, 4294967291ul
	};

	static unsigned long next_size(size_t n)
	{
		unsigned long ans = n;
		for (int i = 0; i < _num_prime; i++)
		{
			if (_prime_list[i] >= n)
			{
				ans = _prime_list[i];
				break;
			}
		}
		return ans;
	}

	Hash _hasher;
	KeyEqual _judgeKeyEqual;
	size_t nodeCnt;
	vector<bucket> buckets;

	//operator=辅助函数
	void exchange(unordered_map& rhs)
	{
		std::swap(_hasher, rhs._hasher);
		std::swap(_judgeKeyEqual, rhs._judgeKeyEqual);
		std::swap(nodeCnt, rhs.nodeCnt);
		std::swap(buckets, rhs.buckets);
	}

public:
	//单向迭代器，只能向前
	class const_iterator;
	class iterator 
	{
		friend class const_iterator;
		friend class unordered_map<Key, T, Hash, KeyEqual>;

	private:
		node* _cur;
		unordered_map* _ascription;

		void exchange(iterator& rhs)
		{
			std::swap(_cur, rhs._cur);
			std::swap(_ascription, rhs._ascription);
		}

	public:
		iterator(node* _p = nullptr, unordered_map* _a = nullptr) :_cur(_p), _ascription(_a) {}
		iterator(const iterator& rhs) :_cur(rhs._cur), _ascription(rhs._ascription) {}
		iterator& operator=(iterator rhs)
		{
			exchange(rhs);
			return *this;
		}
		~iterator() {}

		//march forward,no backward for hash map
		iterator& operator++() 
		{
			const node* old = _cur;
			_cur = _cur->next;
			if (!_cur)
			{
				size_t oriIndex = calcHashIndex(old->elem.first);
				while (!_cur && ++oriIndex < buckets.size())
					_cur = buckets[oriIndex].start;
			}
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		//dereference
		value_type& operator*() const { return _cur->elem; }
		value_type* operator->() const noexcept { return &(operator*()); }

		//operators to check whether two iterators are same (pointing to the same memory).
		bool operator==(const iterator &rhs) const { return _cur == rhs._cur; }
		bool operator==(const const_iterator &rhs) const { return _cur == rhs._cur; }

		bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
		bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }

		//迭代器有效性检测
		bool isAscriptedTo(void* _id) const { return _id && _id == _ascription; }
		bool isValid() const { return _cur && _ascription; }
	};
	class const_iterator
	{
		friend class iterator;
		friend class unordered_map<Key, T, Hash, KeyEqual>;

	private:
		node *_cur;
		unordered_map *_ascription;

		void exchange(const_iterator& rhs)
		{
			std::swap(_cur, rhs._cur);
			std::swap(_ascription, rhs._ascription);
		}
	public:
		const_iterator(node *_p = nullptr, unordered_map *_a = nullptr) :_cur(_p), _ascription(_a) {}
		const_iterator(const const_iterator &rhs) :_cur(rhs._cur), _ascription(rhs._ascription) {}
		const_iterator(const iterator &rhs) :_cur(rhs._cur), _ascription(rhs._ascription) {}
		const_iterator& operator=(const_iterator rhs)
		{
			exchange(rhs);
			return *this;
		}
		~const_iterator() {}

		//march forward,no backward
		const_iterator &operator++()
		{
			const node* old = _cur;
			_cur = _cur->next;
			if (!_cur)
			{
				size_t oriIndex = calcHashIndex(old->elem.first);
				while (!_cur && ++oriIndex < buckets.size())
					_cur = buckets[oriIndex].start;
			}
			return *this;
		}
		const_iterator operator++(int)
		{
			const_iterator tmp = *this;
			++*this;
			return tmp;
		}

		//dereference
		const value_type &operator*() const { return _cur->elem; }
		const value_type *operator->() const noexcept { return &(operator*()); }

		//operators to check whether two iterators are same (pointing to the same memory).
		bool operator==(const iterator &rhs) const { return _cur == rhs._cur; }
		bool operator==(const const_iterator &rhs) const { return _cur == rhs._cur; }

		bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
		bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }

		//迭代器有效性检测
		bool isAscriptedTo(void* _id) const { return _id && _id == _ascription; }
		bool isValid() const { return _cur && _ascription; }
	};

	//iterators for a certain hash value elements, behave like list::iterator
	class local_iterator 
	{
		friend class iterator;
		friend class const_iterator;
	private:
		node *_cur;
		bucket *_ascription;

		void exchange(local_iterator& rhs)
		{
			std::swap(_cur, rhs._cur);
			std::swap(_ascription, rhs._ascription);
		}

	public:
		local_iterator(node *_p = nullptr, bucket *_a = nullptr) :_cur(_p), _ascription(_a) {}
		local_iterator(const local_iterator& rhs) :_cur(rhs._cur), _ascription(rhs._ascription) {}
		local_iterator &operator=(local_iterator rhs)
		{
			exchange(rhs);
			return *this;
		}
		~local_iterator() {}

		//march forward
		local_iterator& operator++() 
		{
			if (_cur)
				cur = cur->next;

			return *this;
		}
		local_iterator operator++(int)
		{
			local_iterator tmp = *this;
			++*this;
			return tmp;
		}

		//dereference
		value_type& operator*() const{ return _cur->elem; }
		value_type* operator->() const noexcept { return &(operator*()); }

		//operators to check whether two iterators are same (pointing to the same memory).
		bool operator==(const iterator &rhs) const { return _cur == rhs._cur; }
		bool operator==(const const_iterator &rhs) const { return _cur == rhs._cur; }

		bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
		bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }

		//迭代器有效性检测
		bool isAscriptedTo(void *_id) const { return _id && _id == _ascription; }
		bool isValid() const { return _cur && _ascription; }

	};
	class const_local_iterator
	{
		friend class iterator;
		friend class const_iterator;
	private:
		node *_cur;
		bucket *_ascription;

		void exchange(local_iterator& rhs)
		{
			std::swap(_cur, rhs._cur);
			std::swap(_ascription, rhs._ascription);
		}

	public:
		const_local_iterator(node *_p = nullptr, bucket *_a = nullptr) :_cur(_p), _ascription(_a) {}
		const_local_iterator(const const_local_iterator& rhs) :_cur(rhs._cur), _ascription(rhs._ascription) {}
		const_local_iterator(const local_iterator& rhs) :_cur(rhs._cur), _ascription(rhs._ascription) {}
		const_local_iterator(const iterator &other) :_cur(rhs._cur), _ascription(nullptr) 
		{
			if (_cur)
			{
				size_t n = calcHashIndex(_cur->elem);
				_ascription = &buckets[n];
			}
		}
		const_local_iterator &operator=(const_local_iterator rhs)
		{
			exchange(rhs);
			return *this;
		}
		~const_local_iterator() {}

		//march forward
		const_local_iterator& operator++()
		{
			if (_cur)
				cur = cur->next;

			return *this;
		}
		const_local_iterator operator++(int)
		{
			const_local_iterator tmp = *this;
			++*this;
			return tmp;
		}

		//dereference
		value_type& operator*() const { return _cur->elem; }
		value_type* operator->() const noexcept { return &(operator*()); }

		//operators to check whether two iterators are same (pointing to the same memory).
		bool operator==(const iterator &rhs) const { return _cur == rhs._cur; }
		bool operator==(const const_iterator &rhs) const { return _cur == rhs._cur; }

		bool operator!=(const iterator &rhs) const { return !operator==(rhs); }
		bool operator!=(const const_iterator &rhs) const { return !operator==(rhs); }

		//迭代器有效性检测
		bool isAscriptedTo(void *_id) const { return _id && _id == _ascription; }
		bool isValid() const { return _cur && _ascription; }
	};

public:
	unordered_map(size_t n = _prime_list[0]) :
		_hasher(),
		_judgeKeyEqual(),
		nodeCnt(0),
		buckets(n, bucket())
	{}
	unordered_map(const unordered_map &other) :
		_hasher(other._hasher),
		_judgeKeyEqual(other._judgeKeyEqual), 
		buckets(other.buckets.size(), bucket()),
		nodeCnt(other.nodeCnt) 
	{
		copy_all_nodes(other);
	}
	~unordered_map() { clear(); }
	unordered_map& operator=(unordered_map other) 
	{
		exchange(other);
		return *this;
	}

	iterator begin() 
	{
		for (size_t i = 0; i < buckets.size(); i++)
			if (buckets[i].start)
				return iterator(buckets[i].start, this);

		return iterator(nullptr, this);
	}
	const_iterator cbegin() { return begin(); }

	iterator end() { return iterator(nullptr, this); }
	const_iterator cend() { return end(); }

	bool empty() const { return nodeCnt == 0; }

	size_t size() const { return nodeCnt; }

	//清除所有node
	void clear() 
	{
		for (size_t i = 0; i < buckets.size(); i++)
		{
			node *p = buckets[i].start;
			node *t = nullptr;
			while (p)
			{
				t = p->next;
				delete p;
				p = t;
			}
			buckets[n].start = nullptr;
			buckets[n].cnt = 0;
		}
		nodeCnt = 0;
	}

	//hashtable插入
	pair<iterator, bool> insert(const value_type &value) 
	{
		resize(nodeCnt + 1);
		return _insert(value);
	}

	//删去指定位置上的元素
	void erase(iterator pos)
	{
		if (!pos.isValid() || !pos.isAscriptedTo(this))
			return;

		size_t n = calcHashIndex(pos->first);
		node *p = buckets[n].start;

		if (p == pos._cur)
		{
			buckets[n].start = p->next;
			delete p;
			--nodeCnt;
		}
		else
		{
			node *pre = p;
			p = p->next;
			while (p && p != pos._cur)
			{
				pre = p;
				p = p->next;

			}

			if (p)
			{
				pre->next = p->next;
				delete p;
				--nodeCnt;
			}
		}
	}
	
	//由于不允许key重复，结果非0即1
	size_t count(const Key &key) const { return find(key) == cend() ? 0 : 1; }

	//查找指定的元素
	//若不存在，则返回end()
	iterator find(const Key &key) 
	{
		size_t index = calcHashIndex(key);
		node* p = buckets[index].start;
		while (p && !_judgeKeyEqual(p->elem.first, key))
			p = p->next;
		
		return iterator(p, this);
	}
	const_iterator find(const Key &key) const 
	{
		size_t index = calcHashIndex(key);
		node* p = buckets[index].start;
		while (p && !_judgeKeyEqual(p->elem.first, key))
			p = p->next;

		return const_iterator(p, this);
	}

	//返回由给定key所map的值
	//若不存在，throw exception
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

	//访问由给定key所map的值
	//若不存在，则插入
	T& operator[](const Key& key) 
	{
		iterator target = find(key);
		if (target == end())
			return insert(value_type(key, T())).first->second;
		else
			return target->second;
	}
	const T& operator[](const Key& key) const 
	{
		const_iterator target = find(key);
		if (target == cend())
			throw index_out_of_bound();
		
		return target->second;
	}
	
	//bucket interfaces
	//returns an iterator to the beginning of the specified bucket.
	local_iterator begin(const size_t &n) { return local_iterator(buckets[n].start, &buckets[n]); }
	const_local_iterator cbegin(const size_t &n) const { return begin(); }

	//returns an iterator to the end of the specified bucket.
	local_iterator end(const size_t &n) { return local_iterator(nullptr, &buckets[n]); }
	const_local_iterator cend(const size_t &n) const { return end(); }

	//the number of buckets
	size_t bucket_count() const { return buckets.size(); }

	//the number of nodes in certain bucket
	size_t bucket_size(const size_t &n) const { return buckets[n].cnt; }

	 //the average number of elements per buckets.
	double load_factor() const { return 1.0*nodeCnt / buckets.size(); }

private:
	//深拷贝对方的所有节点
	void copy_all_nodes(const unordered_map& rhs)
	{
		//深拷贝每一个bucket
		//要求buckets已被初始化！
		for (size_t i = 0; i < rhs.buckets.size(); i++)
		{
			buckets.insert(buckets.end(), bucket());

			node *p = rhs.buckets[i].start;
			while (p)
			{
				bucket[i].start = new node(p->elem, bucket[i].start);
				p = p->next;
			}
		}
	}

	//计算相应元素在hash表中的index
	size_t calcHashIndex(const Key& _key) const { return _hasher(_key) % nodeCnt; }
	size_t calcHashIndex(const Key& _key, size_t n) const { return _hasher(_key) % n; }
	size_t calcHashIndex(const value_type& _val) const { return _hasher(_val.first) % nodeCnt; }
	size_t calcHashIndex(const value_type& _val, size_t n) const { return _hasher(_val.first) % n; }

	//重新配置hash表的长度
	void resize(size_t _new_cnt)
	{
		const size_t _old_n = buckets.size();
		if (_new_cnt > _old_n)
		{
			const n = next_size(_new_cnt);
			vector<bucket> tmp(n);

			for (size_t i = 0; i < _old_n; i++)
			{
				node *p = buckets[i].start;
				while (p)
				{
					//计算在新的hash list中的下标
					size_t _new_index = calcHashIndex(p->elem.first, n);
					
					//按插邻接表的方式从原来的list中取出node，再插到新的list的第一个
					buckets[i].start = p->next;
					p->next = tmp[_new_index].start;
					tmp[_new_index].start = p;
					++tmp[_new_index].cnt;
				}
			}

			std::swap(buckets, tmp);
		}
	}

	//在被resize过的hash表中插入
	pair<iterator, bool> _insert(const value_type &_elem)
	{
		const size_t n = calcHashIndex(_elem);
		node* p = buckets[n].start;
		while (p)
		{
			if (_judgeKeyEqual(p->elem.first, _elem.first))
				return pair<iterator, bool>(iterator(p, this), false);
			p = p->next;
		}
		node *tmp = new node(_elem, buckets[n].start);
		buckets[n] = tmp;
		++nodeCnt;
		return pair<iterator, bool>(iterator(tmp, this), true);
	}
};
}

#endif