/* Created on: Tue Nov 25 16:42:26 +01 2025 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for the allocator's public API
extern int mm_init(void);
extern void *mm_malloc(size_t size);
extern void mm_free(void *ptr);

#define ALLOC_SIZE 24
#define NUM_ALLOCS 3

int main() {
  void *ptrs[NUM_ALLOCS];

  // 1. Initialize the allocator (which now internally calls mem_init)
  if (mm_init() == -1) {
    fprintf(stderr, "Allocator initialization failed.\n");
    return 1;
  }
  printf("Allocator initialized successfully.\n");

  // 2. Perform allocations
  for (int i = 0; i < NUM_ALLOCS; i++) {
    ptrs[i] = mm_malloc(ALLOC_SIZE);
    if (ptrs[i] == NULL) {
      fprintf(stderr, "Allocation %d failed.\n", i + 1);
      return 1;
    }
    // Write data to the allocated block to test functionality
    memset(ptrs[i], 'A' + i, ALLOC_SIZE);
    printf("Allocated block %d at %p (size %u bytes): '%.*s...'\n", i + 1,
           ptrs[i], ALLOC_SIZE, 8, (char *)ptrs[i]);
  }

  // 3. Free the second block (middle block)
  printf("Freeing block 2 at %p...\n", ptrs[1]);
  mm_free(ptrs[1]);

  // 4. Free the first block (should coalesce with the previously freed block if
  // contiguous)
  printf("Freeing block 1 at %p...\n", ptrs[0]);
  mm_free(ptrs[0]);

  // 5. Free the third block
  printf("Freeing block 3 at %p...\n", ptrs[2]);
  mm_free(ptrs[2]);

  printf("All blocks freed. Test complete.\n");
  return 0;
}
