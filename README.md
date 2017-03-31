# Milestone
Containers like ***vector, list, deque, stack, queue, priority\_queue, map*** and ***unordered\_map*** were implemented.  
Also, some generic algorithms like ***sort, list\_sort*** and ***make\_heap*** were done at last.  
All the code were checked with valgrind to ensure that there's no memory leak. Suggestions are welcome!

## Implementation Memo
### vector
&emsp;The key parts of a vector are the ___erase___ and ___insert___ operations, whose time complexity are O(n).  
&emsp;Attention should be paid to the so called ___"deep copy"___ operations when moving the objects inside or using the assignment operator, since objects may have pointers pointing to dynamic memory outside the vector.  
&emsp;To handle this, I generally calls the placement new operator to construct an new object in target address and calls the destructor to destroy the original object later.  
&emsp;Additionally, The ___iterator___ I design can check the validity of itself, and the time complexity is ___O(1)___. That is, given a pointer pointing to a vector, the ___isValid___ function can tell if the iterator belongs to the given vector, and if the iterator is still valid after insert or erase operations were taken, since these operations may cause the vector to allocate a larger memory block and to release the original one.

### list
&emsp;The key parts of a list are the ___erase___ and ___insert___ functions, whose time complexity are O(1). Proper construction, destruction and memory management plays a significant role in the implementation as well.  
&emsp;With an additional node named "___last___" inside each list, it's much easier to indicate the end() position, which is post to the last data node. Also, we benefit a lot from this structure when implementing other member functions.  
&emsp;Although the erase and insert operations on certain data node won't cause iterators pointing to other data nodes to be invalid,  I implement the ___isValid___ function inside the ___iterator___ to check if the iterator belongs to the given list and if the iterator is still valid after inserting or erasing with ___O(n)___ time complexity.

### deque
&emsp;Here the deque was implemented with ___Block List___, which combines the advantages of vector and list and provides ___O(sqrt(n))___ performance for ___search___, ___insert___ and ___erase___ operations.  
&emsp;However, it's frustrating to realize that efficiency and standards are not always compatible. For example, if we want to improve the performance, we can adopt a "lazy" strategy in push and insert operations to get better ___amortized___ performance, where maintain operation is postponed until an access operation is detected. This is just like what were done in Splay or Fibonacci Heap, but things got changed when iterator was introduced. We must follow the basic semantics and requirements of iterator: if there's no insert nor erase operations, existing iterators must be valid! This is impossible if we do following operations:"___auto itA=insert(pos,val); auto itB=begin();___" as __itA__ may become invalid after maintain operations were done in begin(). So, after each operations that related to iterator, we must keep the internal structure stable for next operation, which hindered us from using more advanced tactics to further optimized the performance.  
&emsp;Meanwhile, it's of great importance to keep something ___invariant___ when designing the program. They were helpful in handling corner cases and avoid making mistakes. The "___KISS___" principle should be reminded so that it won't be too complicated to figure them out. Besides, be careful to avoid low-level mistakes like wrong operating direction or wrongly used variable... sigh...

### stack
&emsp;Actually, stack is an ___adapter___ instead of a container. It's based on the ___vector___ or ___list___ so that the ___push___, ___pop___ and ___top___ can be realized easily by calling corresponding functions in specified container. Also, we needn't to worry about the memory management since it has been guaranteed before.  
&emsp;Since the elements inside a stack are not allowed to be accessed, we don't have to provide relevant iterator.

### queue
&emsp;Similarly, queue is also an ___adapter___ instead of a container. As we need to support operations on the front, vector is not suitable any more. Thus, it was implemented with the ___list___ we created before. Operations like ___push___, ___pop___ and ___front___ can be done easily by calling their counterparts in list.  
&emsp;Since the elements inside a queue are not permitted to be traversed, we needn't to provide relevant iterator.

### priority_queue
&emsp;Here I implemented the priority_queue with both ___binary heap___ and ___fibonacci heap___.  The difference is that binary heap supports all the ordinary operations except the ___merge___ operation while fibonacci heap can handle it well.  
&emsp;For a binary heap, when the top element get extracted, a ___percolateDown___ process will be taken to select the new front element and to re-balance the heap. Similarly, a ___percolateUp___ process will be applied after a new element was appended to the back so that it can be properly placed. Both of these two operations have logarithmic complexity, so the time cost of the ___pop___ and ___push___ operations are ___O(log(n))___.  
&emsp;Theoretically, the time consumption of the ___pop___ operation for a fibonacci heap is ___O(log(n))___ and the ___push___ operation is ___O(1)___ since fibonacci heap adopts a "___lazy___" strategy to store new elements. That is, it simply inserts the element into its data list when a new element comes, so the time consumption is constant. However, a ___consolidate___ process will be taken after the root node was extracted, which effectively re-ordered the heap and takes logarithmic time complexity.  
&emsp;Naturally, iterator is not needed for this data structure as the traversal of a heap is meaningless.

### map
&emsp;Although there're many kinds of balance tree can be applied like Binary Tree, AVL, RB, AA, Splay, Treap, Skip List and so on, we use the famous ___Red-Black Tree___ as the internal infrastructure for map. It's important to clarify why we choose RB instead of ___AVL___, which is more balanced than RB.  
&emsp;As we know, the mostly used operations of a map are ___insert___ and ___erase___. After a new node was inserted, the tree may get unbalanced, and the re-balance process will be taken. For both AVL and RB, it will take at most ***2*** times of ***rotation*** in the worst cases. Thus, the time complexity of the insert operation for a map is ___O(1)___.  
&emsp;However, when it comes to the re-balance process for an erase operation, all the nodes ***along*** the path from root to the erased node ___may___ be maintained for AVL while RB needs at most ***3*** times of rotation. Thus, the time complexity of the erase operation for AVL is ___O(log(n))___ while RB is ___O(1)___. Surely, the ___search___ efficiency of AVL is much better than RB since AVL is more stable, but it's a compromise among the search, insert and erase operations in engineering practice to choose RB.  
&emsp;Beside the choice of internal balance tree, designs of ___interface___ with the use of __template__ also define the quality of the program. One thing we need to handle is that the key and the value are ___paired___ while the search routine only knows the key, thus a ___getKey___ functor is needed to extract the key from the pair. Also a compare object is needed for further comparison.  
&emsp;When it comes to implementation, an extra node named ___header___ is set to be the parent node of the root node, and it's left and right pointer points to the min and max node in ___logical___ order so that the increment and decrement progress of an ___iterator___ will be done much easier. Further more, proper construction and destruction of the header also play a vital role in this program so that classes ___without___ default constructor can be accepted as well.

### unordered_map
&emsp;Based on ___hashtable___, this data structure provides ___O(1)___ time complexity to access, insert and erase an element when the load factor is less than 0.5. We adopt the "___separate chaining___" strategy, and the hash function by default is std::hash.  
&emsp;It doesn't take much effort to implement the ___insert___ and ___erase___ functions, and the ___search___ function is rather trivial. But we need to ___resize___ the hashtable dynamically when it grows to keep the ___load factor___ under 1/2. Note that there's no backward-marching for the ___iterator___, special attention should be paid to the forward-marching operation of an iterator when it reaches the margin of two buckets.

### generic algorithms
&emsp;___sort___:  
&emsp;&emsp;Generally, we adopt the ___quick sort___ algorithm and some optimization work were done to handle the extreme cases.  
&emsp;&emsp;Firstly, the program use the so called ___introspective sort___ method to get a rough result. Then the ___insertion sort___ were carried out to get final result. If the operating sequence deteriorated with too much nesting, ___heap sort___ algorithm will be used to eliminate unnecessary recursive calls.  
&emsp;___list_sort___:  
&emsp;&emsp;As we don't have to operate on internal data, I thought of the ___in-place merge___ method. Although the in-place merge method based on list is free of the heavy cost when reversing the data blocks,  it suffers from the expense to traverse to the middle of the list every time when the iteration step is called.   
&emsp;&emsp;Learning from SGI STL, we use a different style of  ___iterative merge sort___ algorithm to effectively sort the elements with ___O(nlog(n))___ time complexity and ___O(1)___ space complexity. In this way, the data nodes were merged in sequence, and the  ___continuously merging___ of sub-sequence acts just like the ___carry-over___ of binary numbers when doing addition. Thus, we avoided unnecessary traversal. The testing results shows that sorting 100,000,000 elements only costs 10+ seconds!   
&emsp;___make_heap___:  
&emsp;&emsp;Just start from the last node that is not leaf, then adjust each sub-tree  until root node is adjusted, so that the property of heap is maintained. The time complexity of this algorithm is ___O(n)___ since each adjust on sub-tree takes ___O(h)___ time consumption, where h stands for the height of the heap.

## Testing Commands
&emsp;Just cd to "___/script/___" and type "___python3 test-XXX.py___ to run corresponding tests. These python scripts are based on following commands:  
&emsp;Compile: g++ -std=c++11 -O2 test/XXX/YYY.cc -o results/XXX/YYY.exe  
&emsp;common check: ./results/XXX/YYY.exe > ./results/XXX/YYY.txt  
&emsp;memory check: ___valgrind___ --tool=memcheck --leak-check=full ./results/XXX/YYY.exe > ./results/XXX/YYY.txt   
&emsp;As the testing scripts don't carry out memory checking work, you need to use valgrind manually or modify related scripts.

## Declaration
The framework and test cases of this project are based on the course project of DS2016, ACM Class, SJTU. I'm full of admiration for the wonderful work they've done!
