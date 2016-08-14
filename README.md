# Milestone
Course project for Data Structure.  
Containers like ***vector, list, deque, stack, queue, priority\_queue, map, and unordered\_map*** were implemented.  
Also, some generic algorithms like ***sort, list\_sort and make\_heap*** were done at last.  
All the code were checked using valgrind to ensure that there's no memory leak. Suggestions are welcom!
## Implementation details
###vector
The key part of a vector is the __erase__ and __insert__ function.  
&emsp;Special attentation should be payed to the so called "deep copy" when moving the objects inside or using the assignment operator, since objects may have pointers pointing to dynamic memory outside the vector.  
&emsp;To handle this, I generally calls the placement new operator to construct an new object in target address and calls the destructor to desturct the original object latter.  
  
&emsp;Additionally, The __iterator__ I design can check validility of itself and the time complexity is O(1).  
&emsp;Given a pointer pointing to a vector, the __isValid__ function can tell if the iterator is ascripted to the given vector, and if the iterator is still valid after insert or erase operations were taken, since these operations may cause the vector to allocate a larger memory block and to release the original one.
## Declaration
The framework of this project is based on the course project of DS2016, ACM Class, SJTU. I'm full of admiration for the wonderful work they've done!
