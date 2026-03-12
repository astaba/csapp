/* Created on: Tue Nov 25 08:30:21 +01 2025 */

// NOTE: The pool of bytes managed by an allocator is explicitly untyped.
// That fact comes as a major drawback when the allocator has to navigate
// its own arena. To overcome this, it is imperative to restrict and
// standardize all pointer movement using macros, which rely on two key
// techniques:
// (1) Cast any block pointer (void* or payload pointer) to char* so we can
//     navigate the arena in byte-sized steps. Macros such as HDRP, FTRP,
//     NEXT_BLKP, and PREV_BLKP all compute displacements using byte counts
//     (e.g., WSIZE, DSIZE), and casting to char* ensures that increments and
//     decrements correspond to raw bytes.
// (2) Once the correct byte address is located, cast to the appropriate
//     type when reading or writing control information. For example,
//     DEREF_UINT, SET_UINT, GET_SIZE, and GET_ALLOC cast to (unsigned int*)
//     because headers and footers are 4-byte words containing size/alloc bits.
// By standardizing pointer arithmetic and type access in macros, we avoid
// undefined behavior and keep all arena navigation consistent and correct.

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Figure 9.43 Basic constants and macros for manipulating the free list. */

/* Basic constants and macros */
#define WSIZE 4             /* Word and header/footer size (bytes) */
#define DSIZE 8             /* Double word size (bytes) */
#define CHUNKSIZE (1 << 12) /* Extend heap by this amount (bytes) */

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* Pack a size and allocated bit into a word */
#define PACK(size, alloc) ((size) | (alloc))

/* Read and write a word at address p */
#define DEREF_UINT(p) (*(unsigned int *)(p))
#define SET_UINT(p, val) (*(unsigned int *)(p) = (val))

/* Read the size and allocated fields from address p */
#define GET_SIZE(p) (DEREF_UINT(p) & ~0x7)
#define GET_ALLOC(p) (DEREF_UINT(p) & 0x1)

/* Given block ptr bp, compute address of its header and footer */
#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* Given block ptr bp, compute address of next and previous blocks */
#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

void mem_init(void);
void *mem_sbrk(int incr);
