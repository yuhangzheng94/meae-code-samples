#include <os-memory.h>


void first_fit(int size, freezone* fz) {
    int found = freelist;
    int previous = -1;

    while ((found != -1) && (heap[found] < size)) {
        previous = found;
        found = heap[found + 1];
    }

    fz->previous = previous;
    fz->found = found;
}

void best_fit(int size, freezone* fz) {
    int found = freelist;
    int previous = -1;
    int current = found;
    int current_previous = previous;

    while ((current != -1)) {
      // if the current free zone fits best, update previous and found
      if ((heap[current] > size) && (heap[current] < heap[found])) {
        previous = current_previous;
        found = current;
      }

      // keep traversing
      current_previous = current;
      current = heap[current + 1];
    }

    fz->previous = previous;
    fz->found = found;
}

void worst_fit(int size, freezone* fz) {
    int found = freelist;
    int previous = -1;
    int current = found;
    int current_previous = previous;

    while ((current != -1)) {
      // if the current free zone fits worst, update previous and found
      if ((heap[current] > size) && (heap[current] > heap[found])) {
        previous = current_previous;
        found = current;
      }

      // keep traversing
      current_previous = current;
      current = heap[current + 1];
    }

    fz->previous = previous;
    fz->found = found;
}


void* heap_malloc(int size) {
    freezone result;                    // free zone found for the allocation
    int allocation_size = size + 1;     // size of the allocated zone
    void *ptr = NULL;                   // pointer to the allocated zone

    find_free_zone(size, &result);

    // Cannot find a free zone
    if (result.found == -1)
        return NULL;

    int i;

    // if the requested memory is not of the exact size as the free zone,
    // partition a new free zone
    int delta_size;
    if ( (delta_size = heap[result.found] - size) > 0) {
      heap[result.found + allocation_size] = delta_size - 1;
      heap[result.found + allocation_size + 1] = heap[result.found + 1];
    }

    // link the previous free zone with the next free zone
    if (result.previous != -1) {
      if (delta_size > 0) {
        heap[result.previous + 1] = result.found + allocation_size;
      } else {
        heap[result.previous + 1] = heap[result.found + 1];
      }
    } else {
      freelist = result.found + allocation_size;
    }

    heap[result.found] = size;

    // add the allocated memory to the array
    for (i = 0; i < HEAP_SIZE/MIN_BLOCK_SIZE; i++) {
      if (allocations[i] == NULL) {
        allocations[i] = &heap[result.found + 1];
        ptr = allocations[i];
        break;
      }
    }

    return ptr;
}


int heap_free(void *dz) {
    // delete the allocated memory from the array
    int idx = ptr2ind(dz);

    int i;
    for (i = 0; i < HEAP_SIZE/MIN_BLOCK_SIZE; i++) {
      if ( allocations[i] == NULL) {  // original
        if (i == HEAP_SIZE/MIN_BLOCK_SIZE - 1) {
          printf("-1\n");
          return -1;
        }
      }
      if (ptr2ind(allocations[i]) == idx) {
        allocations[i] = NULL;
        dz = NULL;
        break;
      }
    }

    // calculate the size of deallocated memory
    int size = heap[idx - 1];
    i = idx;
    while ( (heap[i] != '\0') || (heap[i] == -1) ) {
      heap[i] = 0;
      i++;
    }
    heap[i] = 0;
    int allocation_size = size + 1;
    heap[idx - 1] = size;

    if (idx - 1 < freelist) {
      heap[idx] = freelist;
      freelist = idx - 1;
    }
    // find the previous and next free zone
    int previous = -1;
    int next = -1;
    int current = freelist;

    while (current != -1) {
      if (current < idx - 1) {
        previous = current;
      }
      if (current > idx - 1) {
        next = current;
        break;
      }
      current = heap[current + 1];
    }

    // connect the deallocated memory with next free zone
    if (next != -1) {
      int delta_size;
      if ( (delta_size = next - idx) == size) {
        heap[idx - 1] += heap[next] + 1;
        heap[idx] = heap[next + 1];
        heap[next] = 0;
        heap[next + 1] = 0;
      } else {
        heap[idx] = next;
      }
    }

    // connect the deallocated memory with next free zone
    if (previous != -1) {
      int delta_size;
      int previous_size = heap[previous];
      if ( (delta_size = (idx - 1) - previous) == (previous_size + 1)) {
        heap[previous] += allocation_size;
        heap[previous + 1] = heap[idx];
        heap[idx - 1] = 0;
        heap[idx] = 0;
      } else {
        heap[previous + 1] = idx - 1;
      }
    }

    return 0;
}

int heap_defrag() {
    int i, j, k;

    // find the next allocated memory that needs conpacted
    int next_allocation;
    if ( (freelist == 0) && (heap[1] == -1) ) {
      return 0;
    } else {
      next_allocation = freelist + heap[freelist] + 1;
    }

    int size, next, next_allocation_size;
    while (heap[freelist + 1] != -1) {
      next_allocation_size = heap[next_allocation] + 1;
      size = heap[freelist];
      next = heap[freelist + 1];

      // conpact
      for (j = 0; j < next_allocation_size; j++) {
        heap[freelist + j] = heap[next_allocation + j];
        heap[next_allocation + j] = 0;
      }

      // update pointer to allocated memory
      for (k = 0; k < HEAP_SIZE/MIN_BLOCK_SIZE; k++) {
        if (ptr2ind(allocations[k]) == next_allocation + 1) {
          allocations[k] = &heap[freelist + 1];
          break;
        }
      }

      // update freelist
      freelist += next_allocation_size;
      heap[freelist] = size;
      heap[freelist + 1] = next;

      // consolidate free zones if they touch
      if (next != -1) {
        if (freelist + size + 1 == next) {
          heap[freelist] += heap[next] + 1;
          heap[freelist + 1] = heap[next + 1];
          heap[next] = 0;
          heap[next + 1] = 0;
        }
      }

      // find next allocated memory
      if (heap[freelist + 1] != -1) {
        next_allocation = freelist + heap[freelist] + 1;
      }

    }

    return heap[freelist] + 1;
}
