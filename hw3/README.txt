The basic structure implemented is based on seggregated storage and free lists.

An array of size SEGARRAYSIZE contains a pointer to a doubly linked free list each.
Each linked list in the array holds memory blocks in a certain range. For instance,
the list at index 0 holds free blocks in the range 1-31 bytes, index 1 holds 32-63 bytes,
index 2 holds 64-127 bytes etc. All these ranges are defined in the retrieveIndex() function.
Each free block contains a pointer to the next free block (NEXT_BLOCK_SEG(bp)) and 
a pointer to the previous free block (PREV_BLOCK_SEG(bp)) creating a doubly linked list structure.
The free blocks are ordered in a descending order from high to low memory as you traverse 
through a free list. We found that a descending order had a higher performance than ascending 
order, perhaps, due to a high hit rate on the higher memory blocks in the list. 

The find_fit() function is called by mm_malloc() to find a free block that is a fit for the
size to be allocated. If no free block is found for the size in the free list for it's range,
a free block in a higher memory ranged free list is retrieved, else the heap is extended. 
Once a free block is retrieved for a size, the place() function is called where the free block is 
split into 2 smaller blocks, where one block accomodates for the size being malloced. The other free
block is added into the seggregated free list structure in it's respective range. 

For mm_free(), the coalesce function is called to account for the 4 different cases. 
A freed block is added into a free list in it's range.
