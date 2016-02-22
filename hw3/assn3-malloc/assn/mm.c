/*
 * Following is a description of our dynamic storage allocator.
 *
 * The basic structure implemented is based on seggregated storage and free lists.
 *
 * An array of size SEGARRAYSIZE contains a pointer to a doubly linked free list each.
 * Each linked list in the array holds memory blocks in a certain range. For instance,
 * the list at index 0 holds free blocks in the range 1-31 bytes, index 1 holds 32-63 bytes,
 * index 2 holds 64-127 bytes etc. All these ranges are defined in the retrieveIndex() function.
 * Each free block contains a pointer to the next free block (NEXT_BLOCK_SEG(bp)) and 
 * a pointer to the previous free block (PREV_BLOCK_SEG(bp)) creating a doubly linked list structure.
 * The free blocks are ordered in a descending order from high to low memory as you traverse 
 * through a free list. We found that a descending order had a higher performance than ascending 
 * order, perhaps, due to a high hit rate on the higher memory blocks in the list. 
 *
 * The find_fit() function is called by mm_malloc() to find a free block that is a fit for the
 * size to be allocated. If no free block is found for the size in the free list for it's range,
 * a free block in a higher memory ranged free list is retrieved, else the heap is extended. 
 * Once a free block is retrieved for a size, the place() function is called where the free block is 
 * split into 2 smaller blocks, where one block accomodates for the size being malloced. The other free
 * block is added into the seggregated free list structure in it's respective range. 
 *
 * For mm_free(), the coalesce function is called to account for the 4 different cases. 
 * A freed block is added into a free list in it's range.
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team = {
    /* Team name */
    "Null Terminators",
    /* First member's full name */
    "Sanjana Kambalapally",
    /* First member's email address */
    "sanjana.kambalapally@mail.utoronto.ca",
    /* Second member's full name (leave blank if none) */
    "MD Rayun Mehrab",
    /* Second member's email address (leave blank if none) */
    "rayun.mehrab@mail.utoronto.ca"
};

/*************************************************************************
 * Basic Constants and Macros
 * You are not required to use these macros but may find them helpful.
*************************************************************************/
#define WSIZE       sizeof(void *)            /* word size (bytes) */
#define DSIZE       (2 * WSIZE)            /* doubleword size (bytes) */
#define CHUNKSIZE   (1<<7)      /* initial heap size (bytes) */

#define MAX(x,y) ((x) > (y)?(x) :(y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define GET(p)          (*(uintptr_t *)(p))
#define PUT(p,val)      (*(uintptr_t *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p)     (GET(p) & ~(DSIZE - 1))
#define GET_ALLOC(p)    (GET(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp)        ((char *)(bp) - WSIZE)
#define FTRP(bp)        ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))
#define SEGARRAYSIZE 12

/* Given free block ptr bp, computer address of next and previous free block*/
#define NEXT_BLOCK_SEG(bp) ((char*)bp)
#define PREV_BLOCK_SEG(bp) ((char*)bp + WSIZE)

int retrieveIndex(size_t size);
void removeBlock(void* bp);
void addBlock(void *bp);
void *place_left(void* bp, size_t asize);

void* heap_listp = NULL;
void* seg_array[SEGARRAYSIZE]; // Holds the seggregated free list structure

/****************************************************************************
 * mm_init
 * Initialize the heap, including "allocation" of the prologue and epilogue
 * Initialize the seg_array elements to NULL
 ****************************************************************************/
 int mm_init(void)
 {
   if ((heap_listp = mem_sbrk(4*WSIZE)) == (void *)-1)
         return -1;
     PUT(heap_listp, 0);                         // alignment padding
     PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));   // prologue header
     PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));   // prologue footer
     PUT(heap_listp + (3 * WSIZE), PACK(0, 1));    // epilogue header
     heap_listp += DSIZE;

     /* Initialize seggregated storage array */
     int i;
     for(i=0; i<SEGARRAYSIZE; i++){
        seg_array[i] =NULL;
     }

     return 0;
 }

/**********************************************************
 * coalesce
 * Covers the 4 cases discussed in the text:
 * - both neighbours are allocated
 * - the next block is available for coalescing
 * - the previous block is available for coalescing
 * - both neighbours are available for coalescing
 **********************************************************/
void *coalesce(void *bp)
{
    size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    if (prev_alloc && next_alloc) {       /* Case 1 */
        addBlock(bp);
        return bp;
    }

    else if (prev_alloc && !next_alloc) { /* Case 2 */
        removeBlock(NEXT_BLKP(bp));
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp), PACK(size, 0));
        PUT(FTRP(bp), PACK(size, 0));
        addBlock(bp);
        return (bp);
    }

    else if (!prev_alloc && next_alloc) { /* Case 3 */
        removeBlock(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(FTRP(bp), PACK(size, 0));
        PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
        addBlock(PREV_BLKP(bp));
        return (PREV_BLKP(bp));
    }

    else {            /* Case 4 */
        removeBlock(NEXT_BLKP(bp));
        removeBlock(PREV_BLKP(bp));
        size += GET_SIZE(HDRP(PREV_BLKP(bp)))  +
                GET_SIZE(FTRP(NEXT_BLKP(bp)))  ;
        PUT(HDRP(PREV_BLKP(bp)), PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(size,0));
        addBlock(PREV_BLKP(bp));
        return (PREV_BLKP(bp));
    }
}


/**********************************************************
 * extend_heap
 * Extend the heap by "words" words, maintaining alignment
 * requirements of course. Free the former epilogue block
 * and reallocate its new header
 **********************************************************/
void *extend_heap(size_t words)
{
    char *bp;
    size_t size;

    /* Allocate an even number of words to maintain alignments */
    size = (words % 2) ? (words+1) * WSIZE : words * WSIZE;
    if ( (bp = mem_sbrk(size)) == (void *)-1 )
        return NULL;

    /* Initialize free block header/footer and the epilogue header */
    PUT(HDRP(bp), PACK(size, 0));                // free block header
    PUT(FTRP(bp), PACK(size, 0));                // free block footer
    PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));        // new epilogue header

    /* Coalesce if the previous block was free */
    return coalesce(bp);
}


/***************************************************************************************
 * addBlock
 * Adds a block that is freed to the seggregate storage structure
 * retrieveIndex() returns the index on the seg_array that the block's size falls under.
 * If the list at this index is empty, add the block at the head of the list and assign 
 * it's next and free blocks to 0 as there are no other blocks.
 * Else, traverse through the free list to find the perfect spot to insert the free block.
 * 4 special cases encountered are described below in the code.
 ***************************************************************************************/
void addBlock(void *bp){

    int index = retrieveIndex(GET_SIZE(HDRP(bp)));
    void* prevBlock = NULL;
    void* currBlock = seg_array[index];

    /* Case 1: if free list is empty */
     if(!currBlock)   
    {
        seg_array[index] = bp;
        PUT(NEXT_BLOCK_SEG(bp), 0);
        PUT(PREV_BLOCK_SEG(bp), 0);
        return;
    }

    while(currBlock!=NULL){
        if(GET_SIZE(HDRP(currBlock)) <=  GET_SIZE(HDRP(bp))) {

            /* Add previous and current pointers to the bp block  */
            PUT(NEXT_BLOCK_SEG(bp), (uintptr_t) currBlock);
            PUT(PREV_BLOCK_SEG(bp), (uintptr_t) prevBlock);

            /* Case 2: Insert between a previous and current block */
            if(prevBlock){
                PUT(NEXT_BLOCK_SEG(prevBlock), (uintptr_t) bp);
                PUT(PREV_BLOCK_SEG(currBlock), (uintptr_t) bp);

            /* Case 3: Insert at the head of the list */
            }else if(!prevBlock){
                seg_array[index] = bp;
                PUT(PREV_BLOCK_SEG(currBlock), (uintptr_t) bp);
            }
            return;
        }
        prevBlock = currBlock;
        currBlock = (void*) (GET(NEXT_BLOCK_SEG(currBlock)));
    }

    /* Add previous and current pointers to the bp block */
    PUT(NEXT_BLOCK_SEG(bp), (uintptr_t) currBlock);
    PUT(PREV_BLOCK_SEG(bp), (uintptr_t) prevBlock);

    /* Case 4: Insert at the end of the list */
    if(prevBlock){
        PUT(NEXT_BLOCK_SEG(prevBlock), (uintptr_t) bp);
    }
}

/***************************************************************************************
 * removeBlock
 * Removes a block from the seggregated storage structure.
 * retrieveIndex() returns the index on the seg_array that the block's size falls under.
 * The free list at this index is traversed through in the seg_array.
 * If the block to be removed is at the head of the list, assign the next block in the 
 * free list as the head.
 * Else, remove the block from the list and adjust it.
 * 4 special cases are described below in the code, where the blocks are adjusted.
 ***************************************************************************************/
void removeBlock(void* bp){
    void* nextBlock = (void*)GET(NEXT_BLOCK_SEG(bp));
    void* prevBlock = (void*)GET(PREV_BLOCK_SEG(bp));
    int index = retrieveIndex(GET_SIZE(HDRP(bp)));

    /* Case 1: If the block is the head of the list, make the next block the head*/
    if(seg_array[index] == bp){
        seg_array[index] = (void*) nextBlock;
    }

    /* Case 2: If only previous block is present */
    if(prevBlock && !nextBlock){
        PUT(NEXT_BLOCK_SEG(prevBlock), GET(NEXT_BLOCK_SEG(bp)));

    /* Case 3: If only next block is present */
    }else if(!prevBlock && nextBlock){
        PUT(PREV_BLOCK_SEG(nextBlock), GET(PREV_BLOCK_SEG(bp)));

    /* Case 4: If both previous and next blocks are present */
    }else if(prevBlock && nextBlock){
        PUT(NEXT_BLOCK_SEG(prevBlock), GET(NEXT_BLOCK_SEG(bp)));
        PUT(PREV_BLOCK_SEG(nextBlock), GET(PREV_BLOCK_SEG(bp)));
    }
}


/***************************************************************************************
 * retrieveIndex
 * Returns the index of the array where size falls under
 * Index 0, contains a range from 1 - 32bytes
 * Index 1, contains a range from 32 - 63 bytes
 * Index 2, contains a range from 64 - 127 bytes
 * etc.
 * Any size greater that 32767 bytes, falls in index 11 of the array.
 ***************************************************************************************/
int retrieveIndex(size_t size){
    if(size<=31){
        return 0;
    }else if(size<=63){
        return 1;
    }else if(size<=127){
        return 2;
    }else if(size<=255){
        return 3;
    }else if(size<=511){
        return 4;
    }else if(size<=1023){
        return 5;
    }else if(size<=2047){
        return 6;
    }else if(size<=4095){
        return 7;
    }else if(size<=8191){
        return 8;
    }else if(size<=16383){
        return 9;
    }else if(size<=32767){
        return 10;
    }else{
        return 11;
    }    
}

/************************************************************************************
 * find_fit 
 * Traverses through the seg_array to check if a free block is present in the list
 * and the free block present at the head is large enough to accomodate for asize.
 * This is possible as the free blocks in the free list are arranged in a descending 
 * order of memory.
 * Else, check if any free blocks are present in the next range of the seg_array
 * Return NULL if there are no free blocks in the seggregated storage structure
 ************************************************************************************/
void * find_fit(size_t asize)
{
    int index = retrieveIndex(asize);

    /* Iterate through seg_array*/
    while(index < SEGARRAYSIZE){

        /* Check if the first free block is present and is large enough for asize */
        if (seg_array[index] && (asize <= GET_SIZE(HDRP(seg_array[index]))))
        {
            return seg_array[index];
        }

        index++;
    }
    return NULL;
}

/***************************************************************
 * place
 * If sizeLeft is large enough, 
 * split a free block into 2 blocks of size asize and sizeLeft, 
 * Insert the block of sizeleft back into the free list.
 * Mark the other block as allocated. 
 ***************************************************************/
void *place(void* bp, size_t asize)
{
    size_t sizeLeft = (size_t)GET_SIZE(HDRP(bp)) - asize;
    removeBlock(bp);
    if(sizeLeft >= (2*DSIZE)){
        PUT(HDRP(bp), PACK(sizeLeft, 0));
        PUT(FTRP(bp), PACK(sizeLeft, 0));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(asize, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(asize, 0));
        addBlock(bp);
        bp = NEXT_BLKP(bp);
    }
    /* Get the current block size */
    size_t bsize = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(bsize, 1));
    PUT(FTRP(bp), PACK(bsize, 1));

    return bp;
}

/**********************************************************
 * place_left
 * Used only for reallocation
 * When called, place_left takes pointer bp to an allocated
   block, and splits it so that only data of size 'asize'
   is preserved and placed on the left.
 * The right side of the split is then coalesced with any
   neighbouring free block to the right.
 * The coalesce function also adds it to the segregated
   free list so no need to call addBlock.
 * Splitting is only done when sizeofbp - asize is bigger
   than the minimum block size.
 * Otherwise, at header and footer, the size is set to the
   original size of the block, and then returned.
***********************************************************/
void *place_left(void* bp, size_t asize)
{
    size_t sizeLeft = (size_t)GET_SIZE(HDRP(bp)) - asize;

    if(sizeLeft >= (2*DSIZE)){
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        PUT(HDRP(NEXT_BLKP(bp)), PACK(sizeLeft, 0));
        PUT(FTRP(NEXT_BLKP(bp)), PACK(sizeLeft, 0));
        coalesce(NEXT_BLKP(bp));
    }

    /* Get the current block size */
    size_t bsize = GET_SIZE(HDRP(bp));

    PUT(HDRP(bp), PACK(bsize, 1));
    PUT(FTRP(bp), PACK(bsize, 1));

    return bp;
}

/**********************************************************
 * mm_free
 * Free the block and coalesce with neighbouring blocks
 **********************************************************/
void mm_free(void *bp)
{
    if(bp == NULL){
      return;
    }
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size,0));
    PUT(FTRP(bp), PACK(size,0));
    coalesce(bp);
}



/**********************************************************
 * mm_malloc
 * Allocate a block of size bytes.
 * The type of search is determined by find_fit
 * The decision of splitting the block, or not is determined
 *   in place(..)
 * If no block satisfies the request, the heap is extended
 **********************************************************/
void *mm_malloc(size_t size)
{

    size_t asize; /* adjusted block size */
    size_t extendsize; /* amount to extend heap if no fit */
    char * bp;

    /* Ignore spurious requests */
    if (size == 0)
        return NULL;

    /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1))/ DSIZE);

    /* Search the free list for a fit */
    if ((bp = find_fit(asize)) != NULL) {
        bp = place(bp, asize);
        return bp;
    }

    /* No fit found. Get more memory and place the block */
    extendsize = MAX(asize, CHUNKSIZE);
    if ((bp = extend_heap(extendsize/WSIZE)) == NULL)
        return NULL;
    bp = place(bp, asize);
    return bp;

}

/**********************************************************
 * mm_realloc
 * If size is 0, call mm_free.
 * If ptr is null, call mm_malloc.
 * Otherwise, to reduce expensive operations, in-place
   reallocation is preferred.
 * There are three cases where this can be done.
 * Case 1 - Reallocation size is less than the old size.
   In this case, just split the block (if it can be done)
   using place_left which eliminates copying the data.
 * Case 2 - Reallocation size is more than the old size;
   however, the next block is a free block and old size +
   next block size is bigger than reallocation size. In
   this case, first merge the two blocks into one with
   dirty bit set to allocated. Then, call place_left
   which will split them back but with enough size
   reallocated as requested.
 * Case 3 - Reallocation size is more than the old size;
   however, the next block is the epilogue block of the
   heap. In that case, call extend_heap to extend by the
   difference, then merge the new free block with the
   current block and call place_left again to split them
   back with the block reallocated with the requested
   size.
 * For all other scenarios, we call mm_malloc to allocate
   the required block, then use memcpy to copy the data
   over.
 *********************************************************/
void *mm_realloc(void *ptr, size_t size)
{
    /* If size == 0 then this is just free, and we return NULL. */
    if(size == 0){
      mm_free(ptr);
      return NULL;
    }
    /* If oldptr is NULL, then this is just malloc. */
    if (ptr == NULL)
      return (mm_malloc(size));

    void *oldptr = ptr;
    void *newptr;
    void *extptr;
    size_t copySize;
    size_t asize;
    size_t csize;

    size_t nextSize = GET_SIZE(HDRP(NEXT_BLKP(oldptr)));
    size_t currSize = GET_SIZE(HDRP(oldptr));

   /* Ignore spurious requests. */
    if (size == currSize)
        return oldptr;

   /* Adjust block size to include overhead and alignment reqs. */
    if (size <= DSIZE)
        asize = 2 * DSIZE;
    else
        asize = DSIZE * ((size + (DSIZE) + (DSIZE-1))/ DSIZE);

    /* Case 1. Check if asize <= currSize first. */
    if (asize <= currSize){
        newptr = place_left(oldptr, asize);
        return newptr;
    }
    /* Case 2. Check if asize <= currSize + nextSize and next block is free first. */
    else if ((asize <= currSize + nextSize) && !GET_ALLOC(HDRP(NEXT_BLKP(oldptr)))){
        removeBlock(NEXT_BLKP(oldptr));
        PUT(HDRP(oldptr), PACK(currSize + nextSize, 1));
        PUT(FTRP(oldptr), PACK(currSize + nextSize, 1));
        newptr = place_left(oldptr, asize);
        return newptr;
    }
   /* Case 3. Check if nextSize is 0 first, which would mean it's the epilogue block. */
    else if (nextSize == 0){
        csize = asize - currSize;
        extptr = extend_heap(csize);
        if (extptr == NEXT_BLKP(oldptr)){
            nextSize = GET_SIZE(HDRP(extptr));
            removeBlock(extptr);
            PUT(HDRP(oldptr), PACK(currSize + nextSize, 1));
            PUT(FTRP(oldptr), PACK(currSize + nextSize, 1));
            newptr = place_left(oldptr, asize);
            return newptr;
        }
    }

    /* None of the cases above - malloc the block and copy the data to it. */

    newptr = mm_malloc(size);
    if (newptr == NULL)
      return NULL;

    /* Copy the old data. */
    copySize = GET_SIZE(HDRP(oldptr));
    if (size < copySize)
      copySize = size;
    memcpy(newptr, oldptr, copySize);
    mm_free(oldptr);
    return newptr;
}

/**********************************************************
 * mm_check
 * Check the consistency of the memory heap
 * Return nonzero if the heap is consistent.
 *********************************************************/
int mm_check(void){

    void* ep;
    /*Checking if every free block in heap also in free list */
    void* start = heap_listp;

    while (GET_SIZE(HDRP(start)) != 0)
    {
        printf("HEAP: Size: %d, Allocated: %d\n",GET_SIZE(HDRP(start)),GET_ALLOC(HDRP(start)));
        /* Checking if every free block on heap is also in free list */
        if (!GET_ALLOC(HDRP(start))){
            int index = retrieveIndex(GET_SIZE(HDRP(start)));
            void* seg;
            while (seg!= NULL){
                if (start == seg)
                    break;
                seg = NEXT_BLOCK_SEG(seg);
            }
            if (start != seg)
                return 0;
        }
        start = NEXT_BLKP(start);
        /* Setting max_heap pointer value */
        if (GET_SIZE(HDRP(start)) == 0)
            ep = start;
    }

    /* Iterating through the free list */
    int i;
    for(i=0; i<SEGARRAYSIZE;i++){
        void* currBlock = seg_array[i];
        while (currBlock != NULL)
        {
            printf("FREE LIST: Index: %d, Size:%d\n",i,GET_SIZE(FTRP(currBlock)));
            /* Checking if every block on free list is also marked as free. */
            if (GET_ALLOC(FTRP(currBlock)))
                return 0;
            /* Checking if free blocks are coalesced properly */
            if (!GET_ALLOC(FTRP(NEXT_BLKP(currBlock))))
                return 0;
            /* Checking if pointers for each free list block is a valid pointer in the range of the heap addresses. */
            if (currBlock > ep || currBlock < heap_listp)
                return 0;
            currBlock = (void*) (GET(NEXT_BLOCK_SEG(currBlock)));
        }
    }

  return 1;
}


