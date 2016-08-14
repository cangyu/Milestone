# Milestone
Course project for Data Structure.  
Containers like vector, list, deque, stack, queue, priority\_queue, map, and unordered\_map were implemented.  
Also, some generic algorithms like sort, list\_sort and make\_heap were done at last.  
I think they've covered the majority of the STL implementation though I haven't take advanced consideration on memory pool or related allocator.
All the code were checked using valgrind to ensure that there's no memory leak, hope to make them better!  
Suggestions and advices are warmly welcom!
## Implementation details
###vector
The key part of a vector is the erase and insert  
The key point to implement a vector is to ensure that all the objects within are constructed properly and will be destructed at last. Thus, we should  
pay attentation to the so called "deep copy", which means an object may contain pointe
## Declaration
The framework of this project is based on the course project of DS2016, ACM Class, SJTU. I'm full of admiration for the wonderful work they've done!
