#ifndef SJTU_ALGORITHM_HPP
#define SJTU_ALGORITHM_HPP

#include "exceptions.hpp"
#include "vector.hpp"
#include "list.hpp"

#include <algorithm>

namespace sjtu 
{
	const static size_t _threshold = 16;

template<class VecIter>
void sort(VecIter beg, VecIter end) 
{

}

template<class VecIter, class Compare>
void sort(VecIter beg, VecIter end, Compare comp) 
{
	if (beg != end)
	{
		//call IntroSort

		//final Insertion Sort
	}
}

//For better understanding the idea in STL,
//refer to https://www.zhihu.com/question/31478115/answer/52104149
template<class T>
list<T> list_sort(typename list<T>::iterator beg, typename list<T>::iterator end) 
{
	return list_sort(beg,end, std::less<T>);
}

template<class T, class Compare>
list<T> list_sort(typename list<T>::iterator beg, typename list<T>::iterator end, Compare comp)
{
	list<T> counter[64];
	auto r=beg.cur->prev;
	int srcIndex=0;

	while(beg!=end)
	{
		auto t=beg.cur;
		++beg;

		//extract
		t->prev->next=t->next;
		t->next->prev=t->prev;
		t->prev=t->next=t;

		//insert to counter[0]
		insert_single(t,counter[0].last);

		//merge
		while(!counter[srcIndex+1].empty())
		{
			//merge counter[srcIndex] and counter[srcIndex+1] to counter[srcIndex+1]
			auto e1=counter[srcIndex].last;
			auto e2=counter[srcIndex+1].last;
			auto p1=e1->next;
			auto p2=e2->next;
			auto tmp=p1;

			while(p1!=e1 && p2!=e2)
			{
				if(comp(*p1,*p2))
				{
					tmp=p1;
					p1=p1->next;

					--counter[srcIndex].elemCnt;
					++counter[srcIndex+1].elemCnt;
				}
				else
				{
					tmp=p2;
					p2=p2->next;
				}

				//extract
				tmp->prev->next=tmp->next;
				tmp->next->prev=tmp->prev;
				tmp->prev=tmp->next=tmp;

				//insert to the back of counter[srcIndex+1]
				insert_behind(e2,tmp);
			}

			while(p1!=e1)
			{
				tmp=p1;
				p1=p1->next;

				--counter[srcIndex].elemCnt;
				++counter[srcIndex+1].elemCnt;

				//extract
				tmp->prev->next=tmp->next;
				tmp->next->prev=tmp->prev;
				tmp->prev=tmp->next=tmp;

				//insert to the back of counter[srcIndex+1]
				insert_behind(e2,tmp);
			}

			while(p2!=e2)
			{
				tmp=p2;
				p2=p2->next;

				//extract
				tmp->prev->next=tmp->next;
				tmp->next->prev=tmp->prev;
				tmp->prev=tmp->next=tmp;

				//insert to the back of counter[srcIndex+1]
				insert_behind(e2,tmp);
			}

			//next round operation
			++srcIndex;
		}

		//move counter[srcIndex] to counter[srcIndex+1]
		std::swap(counter[srcIndex].elemCnt, counter[srcIndex+1].elemCnt);
		std::swap(counter[srcIndex].last, counter[srcIndex+1].last);
	}

	//re-link results to beg and end
	auto p=counter[srcIndex].last;

	r->next=p->next;
	p->prev->next=end.cur;
	end.cur->prev=p->prev;
	p->prev->prev=r;

	p->prev=p->next=p;
	counter[srcIndex].elemCnt=0;
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
