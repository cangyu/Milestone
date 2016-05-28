#ifndef SJTU_ALGORITHM_HPP
#define SJTU_ALGORITHM_HPP

#include "exceptions.hpp"

#include "vector.hpp"
#include "list.hpp"

namespace sjtu {

template<class VecIter>
void sort(VecIter beg, VecIter end) {}

template<class VecIter, class Compare>
void sort(VecIter beg, VecIter end, Compare comp) {}

template<class T>
list<T> list_sort(typename list<T>::iterator beg, typename list<T>::iterator end) {}

template<class T, class Compare>
list<T> list_sort(typename list<T>::iterator beg, typename list<T>::iterator end, Compare comp) {}

template<class VecIter>
void make_heap(VecIter beg, VecIter end) {}

template<class VecIter, class Compare>
void make_heap(VecIter beg, VecIter end, Compare comp) {}

}

#endif