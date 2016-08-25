#ifndef SJTU_ALGORITHM_HPP
#define SJTU_ALGORITHM_HPP

#include "exceptions.hpp"
#include "vector.hpp"

#include <functional>
#include <cmath>
#include <algorithm>

namespace sjtu
{
	template<typename T>
	const T& _median_of_three(const T &a, const T &b, const T &c)
	{
		return a < b ? (b < c ? b : (a < c ? c : a)) : (a < c ? a : (b < c ? c : b));
	}

	template<class VecIter, class T>
	VecIter _partition(VecIter first, VecIter last, T pivot)
	{
		for(;;)
		{
			while (*first < pivot)
				++first;
			do { --last; } while (pivot < *last);

			if (last - first <= 0)
				return first;

			std::swap(first, last);
			++first;
		}
	}

	template<class VecIter, class Compare>
	void _introspective_sort(VecIter beg, VecIter end, Compare comp, int depth_limit)
	{
		const static size_t _threshold = 16;

		while (end - beg > _threshold)
		{
			if (depth_limit <= 0)
				_heap_sort(beg, end, comp);

			--depth_limit;

			VecIter cut = _partition(beg, end, _median_of_three(*beg, *(beg + (end - beg) / 2), *(end - 1)));
			_introspective_sort(cut, end, depth_limit);//对右半段进行递归
			end = cut;//回到while继续处理左半段
		}
	}

	template<class VecIter, class Compare>
	void _insertion_sort(VecIter first, VecIter last, Compare comp)
	{
		if (first == last) return;

		for (auto t = first + 1; t != last; ++t)
		{
			auto val = *t;
			if (comp(val, *first))
			{
				for (auto p = t; p != first; --p)
				{
					*p = *(p - 1);
					(p - 1)->~(typename VecIter::value_type)();
				}
				*first = val;
			}
			else
			{
				auto next = last;
				--next;
				while (comp(val, *next))
				{
					*last = *next;
					last = next;
					--next;
				}
				*last = val;
			}
		}
	}

	template<class VecIter, class Compare>
	void sort(VecIter beg, VecIter end, Compare comp)
	{
		if (beg != end)
		{
			_introspective_sort(beg, end, comp, 2 * (int)(std::floor(std::log2(end - beg))));
			_insertion_sort(beg, end, comp);
		}
	}

	template<class VecIter>
	void sort(VecIter beg, VecIter end)
	{
		sort(beg, end, std::less<typename VecIter::value_type>());
	}

	template<class VecIter>
	void make_heap(VecIter beg, VecIter end)
	{
		const int len = end - beg;
		if (len <= 1) return;

		for (int hole = (len - 2) / 2; hole >= 0; --hole)
		{
			int curStart = hole;
			int child = 2 * curStart + 1;
			while (child < len)
			{
				if (child + 1 < len && *(beg + child) < *(beg + child + 1))
					++child;

				if (*(beg + curStart) < *(beg + child))
				{
					std::swap(*(beg + curStart), *(beg + child));
					curStart = child;
					child = 2 * curStart + 1;
				}
				else
					break;
			}
		}
	}

	template<class VecIter, class Compare>
	void make_heap(VecIter beg, VecIter end, Compare comp)
	{
		const int len = end - beg;
		if (len <= 1) return;

		for (int hole = (len - 2) / 2; hole >= 0; --hole)
		{
			int curStart = hole;
			int child = 2 * curStart + 1;
			while (child < len)
			{
				if (child + 1 < len && comp(*(beg + child + 1), *(beg + child)))
					++child;

				if (comp(*(beg + child), *(beg + curStart)))
				{
					std::swap(*(beg + curStart), *(beg + child));
					curStart = child;
					child = 2 * curStart + 1;
				}
				else
					break;
			}
		}
	}

	template<class VecIter, class Compare>
	void _heap_sort(VecIter first, VecIter last, Compare comp)
	{

	}

}

#endif
