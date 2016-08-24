#ifndef SJTU_ALGORITHM_HPP
#define SJTU_ALGORITHM_HPP

#include "exceptions.hpp"
#include "vector.hpp"

namespace sjtu 
{
template<class VecIter>
void sort(VecIter beg, VecIter end) 
{

}

template<class VecIter, class Compare>
void sort(VecIter beg, VecIter end, Compare comp) 
{
	const static size_t _threshold = 16;

	if (beg != end)
	{
		//call IntroSort

		//final Insertion Sort
	}
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

}

#endif
