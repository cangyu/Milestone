# Milestone
Course project for Data Structure.  
Containers like ***vector, list, deque, stack, queue, priority\_queue, map*** and ***unordered\_map*** were implemented.  
Also, some generic algorithms like ***sort, list\_sort*** and ***make\_heap*** were done at last.  
All the code were checked using valgrind to ensure that there's no memory leak. Suggestions are welcom!
## Implementation details
###vector
&emsp;The key part of a vector is the ___erase___ and ___insert___ operation, whose time complexity are O(n).  
&emsp;Special attentation should be paid to the so called ___"deep copy"___ operations when moving the objects inside or using the assignment operator, since objects may have pointers pointing to dynamic memory outside the vector.  
&emsp;To handle this, I generally calls the placement new operator to construct an new object in target address and calls the destructor to desturct the original object later.  
  
&emsp;Additionally, The ___iterator___ I design can check the validility of itself, and the time complexity is ___O(1)___. That is, given a pointer pointing to a vector, the ___isValid___ function can tell if the iterator is ascripted to the given vector, and if the iterator is still valid after insert or erase operations were taken, since these operations may cause the vector to allocate a larger memory block and to release the original one.
###list
&emsp;The key part of a list is the ___erase___ and ___insert___ function.  
## Declaration
The framework of this project is based on the course project of DS2016, ACM Class, SJTU. I'm full of admiration for the wonderful work they've done!
