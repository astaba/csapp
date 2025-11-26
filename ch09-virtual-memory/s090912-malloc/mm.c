/* Created on: Mon Nov 24 15:36:39 +01 2025 */

#include "memlib.h"

static char *heap_listp = NULL;

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t asize);
static void place(void *bp, size_t asize);

/* Figure 9.44: mm_init creates a heap with an initial free block. */

int mm_init(void) {
  /*
   * FIX: Call the underlying memory system initializer first.
   * mem_sbrk depends on mem_init having set up mem_heap, mem_brk, and
   * mem_max_addr.
   */
  if (heap_listp == NULL)
    mem_init();

  /* Create the initial empty heap */
  if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1)
    return -1;
  SET_UINT(heap_listp, 0);                            /* Alignment padding */
  SET_UINT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1)); /* Prologue header */
  SET_UINT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1)); /* Prologue footer */
  SET_UINT(heap_listp + (3 * WSIZE), PACK(0, 1));     /* Epilogue header */
  heap_listp += (2 * WSIZE); /* Point to Prologue bp (payload) */

  /* Extend the empty heap with a free block of CHUNKSIZE bytes
   * Notice: extend_heap() expects the number of words not bytes */
  if (extend_heap(CHUNKSIZE / WSIZE) == NULL)
    return -1;
  return 0;
}

/* Figure 9.45 extend_heap extends the heap with a new free block. */

static void *extend_heap(size_t words) {
  char *bp;
  size_t size;

  /* Allocate an even number of words to maintain alignment */
  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
  if ((long)(bp = mem_sbrk(size)) == -1)
    return NULL;

  /* Initialize free block header/footer and the epilogue header */
  SET_UINT(HDRP(bp), PACK(size, 0));         /* Free block header */
  SET_UINT(FTRP(bp), PACK(size, 0));         /* Free block footer */
  SET_UINT(HDRP(NEXT_BLKP(bp)), PACK(0, 1)); /* New epilogue header */

  /* Coalesce if the previous block was free */
  return coalesce(bp);
}

/* Figure 9.46 mm_free frees a block and uses boundary-tag coalescing to merge
 * it with any adjacent free blocks in constant time.
 * */

void mm_free(void *bp) {
  size_t size = GET_SIZE(HDRP(bp));

  SET_UINT(HDRP(bp), PACK(size, 0));
  SET_UINT(FTRP(bp), PACK(size, 0));
  coalesce(bp);
}

static void *coalesce(void *bp) {
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if (prev_alloc && next_alloc) { /* Case 1 */
    return bp;
  }

  else if (prev_alloc && !next_alloc) { /* Case 2 */
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    SET_UINT(HDRP(bp), PACK(size, 0));
    SET_UINT(FTRP(bp), PACK(size, 0));
  }

  else if (!prev_alloc && next_alloc) { /* Case 3 */
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    SET_UINT(FTRP(bp), PACK(size, 0));
    SET_UINT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }

  else { /* Case 4 */
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
    SET_UINT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    SET_UINT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }
  return bp;
}

/* Figure 9.47 mm_malloc allocates a block from the free list. */

void *mm_malloc(size_t size) {
  size_t asize;      /* Adjusted block size */
  size_t extendsize; /* Amount to extend heap if no fit */
  char *bp;

  /* Ignore spurious requests */
  if (size == 0)
    return NULL;

  /* Adjust block size to include overhead and alignment reqs. */
  /* NOTE: This "asize" definition is crucial: (1) by adding DSIZE we ensure
   * enough spaces for a preceding one-word header and a tailing one-word
   * footer;(2) by adding (DSIZE - 1) we ensure that whatever size the user
   * requests, is it always contain within the next double-word boundary. As a
   * result the returned "bp" always points right to the payload, preceded by
   * its header, terminating either right on a double-word boundary or within a
   * safe alignment padding and followed by the footer. */

  if (size <= DSIZE)
    asize = 2 * DSIZE;
  else
    asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);

  /* Search the free list for a fit */
  if ((bp = find_fit(asize)) != NULL) {
    place(bp, asize);
    return bp;
  }

  /* No fit found. Get more memory and place the block */
  extendsize = MAX(asize, CHUNKSIZE);
  if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
    return NULL;
  place(bp, asize);
  return bp;
}

/* Practice Problem 9.8 (solution page 920) */
/* Implement a find_fit function for the simple allocator described in
 * Section 9.9.12. Your solution should perform a first-fit search of the
 * implicit free list. */

static void *find_fit(size_t asize) {
  size_t sizefit;
  char *bp = heap_listp;

  while ((sizefit = GET_SIZE(HDRP(bp))) != 0) {
    if (!GET_ALLOC(HDRP(bp)) && (sizefit >= asize))
      return bp;
    bp = NEXT_BLKP(bp);
  }

  return NULL;
}

/* Practice Problem 9.9 (solution page 920) */
/* Implement a place function for the example allocator. Your solution should
 * place the requested block at the beginning of the free block, splitting only
 * if the size of the remainder would equal or exceed the minimum block size. */

static void place(void *bp, size_t asize) {
  size_t diff = GET_SIZE(HDRP(bp)) - asize;

  if (diff >= (2 * DSIZE)) {
    // Lay down one split for the requested block.
    SET_UINT(HDRP(bp), PACK(asize, 1));
    SET_UINT(FTRP(bp), PACK(asize, 1));
    // Lay down another split for the remainder block.
    SET_UINT(HDRP(NEXT_BLKP(bp)), PACK(diff, 0));
    SET_UINT(FTRP(NEXT_BLKP(bp)), PACK(diff, 0));
  } else {
    SET_UINT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
    SET_UINT(FTRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
  }
}
