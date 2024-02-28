# Memory Allocation Project

This project implements a memory allocation system in C, that i made as part of my Operating systems 2 course.

## Overview

The project consists of two files:
- `heap.h`: Header file containing function prototypes and struct definitions.
- `heap.c`: Source file containing the implementation of memory allocation functions.

## Functions

### `heap_setup()`

Initializes the memory manager.

### `heap_clean()`

Frees all allocated memory and resets the memory manager.

### `heap_malloc(size_t size)`

Allocates a block of memory of the given size.

### `heap_calloc(size_t number, size_t size)`

Allocates memory for an array of elements, initialized to zero.

### `heap_realloc(void *memblock, size_t count)`

Changes the size of the memory block pointed to by `memblock`.

### `heap_free(void *memblock)`

Frees the memory block pointed to by `memblock`.

### `heap_validate()`

Validates the integrity of the heap.

### `get_pointer_type(const void *const pointer)`

Determines the type of the pointer.

### `heap_get_largest_used_block_size()`

Returns the size of the largest used memory block.

## Usage

To use the memory allocation functions, include `heap.h` in your source files and link with `heap.c`.

## Example

```c
#include "heap.h"
#include <stdio.h>

int main() {
    heap_setup();
    
    // Example usage
    void *ptr = heap_malloc(10);
    heap_free(ptr);
    
    heap_clean();
    
    return 0;
}
