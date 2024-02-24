#include "heap.h"

struct memory_manager_t manager;

int heap_setup(void) {
    manager.memory_size = 0;
    manager.memory_start = custom_sbrk(0);
    if (manager.memory_start == NULL) {
        return -1;
    }
    manager.first_memory_chunk = NULL;
    return 0;
}

void heap_clean(void) {
    custom_sbrk((-1) * (int) manager.memory_size);
    manager.memory_size = 0;
    manager.memory_start = NULL;
    manager.first_memory_chunk = NULL;
}

void *heap_malloc(size_t size) {
    if (size <= 0) {
        return NULL;
    }
    if (manager.first_memory_chunk == NULL) {
        void *temp = custom_sbrk(0);
        void *p = custom_sbrk((intptr_t) (sizeof(struct memory_chunk_t) + 8 + (intptr_t) size + 8));
        if (p != temp) {
            return NULL;
        }
        struct memory_chunk_t *head = (struct memory_chunk_t *) p;

        head->next = NULL;
        head->size = size;
        head->prev = NULL;
        manager.first_memory_chunk = head;

        char *front_fence = (char *) p + sizeof(struct memory_chunk_t);

        for (int i = 0; i < 8; ++i) {
            *(front_fence + i) = '#';
        }

        char *back_fence = (char *) p + sizeof(struct memory_chunk_t) + 8 + size;

        for (int i = 0; i < 8; ++i) {
            *(back_fence + i) = '#';
        }

        manager.memory_size += (sizeof(struct memory_chunk_t) + 8 + (intptr_t) size + 8);

        return (void *) ((unsigned char *) p + sizeof(struct memory_chunk_t) + 8);
    } else {
        struct memory_chunk_t *cur = manager.first_memory_chunk;

        if (cur != manager.memory_start) {
            int space = (int) ((char *) manager.first_memory_chunk - (char *) manager.memory_start);
            if (space >= (int) (sizeof(struct memory_chunk_t) + 8 + size + 8)) {
                struct memory_chunk_t *head = (struct memory_chunk_t *) manager.memory_start;
                head->next = manager.first_memory_chunk;
                manager.first_memory_chunk->prev = head;
                manager.first_memory_chunk = head;
                head->prev = NULL;
                head->size = size;
                char *front_fences = (char *) ((char *) head + sizeof(struct memory_chunk_t));
                for (int i = 0; i < 8; ++i) {
                    *(front_fences + i) = '#';
                }

                char *back_fences = (char *) ((char *) head + sizeof(struct memory_chunk_t) + 8 + size);
                for (int i = 0; i < 8; ++i) {
                    *(back_fences + i) = '#';
                }
                return (char *) ((char *) head + sizeof(struct memory_chunk_t) + 8);
            }
        }

        while (cur->next != NULL) {
            int max_space = (int) ((char *) cur->next - (char *) cur - sizeof(struct memory_chunk_t) - 8 - cur->size -
                                   8);
            if (max_space >= (int) (sizeof(struct memory_chunk_t) + 8 + size + 8)) {
                struct memory_chunk_t *new = (struct memory_chunk_t *) ((char *) cur + sizeof(struct memory_chunk_t) +
                                                                        8 + cur->size + 8);
                new->prev = cur;
                new->next = cur->next;
                cur->next->prev = new;
                cur->next = new;
                new->size = size;
                char *front_fence = (char *) ((char *) new + sizeof(struct memory_chunk_t));
                for (int i = 0; i < 8; ++i) {
                    *(front_fence + i) = '#';
                }
                char *back_fence = (char *) ((char *) new + sizeof(struct memory_chunk_t) + 8 + size);
                for (int i = 0; i < 8; ++i) {
                    *(back_fence + i) = '#';
                }
                return (void *) ((char *) new + sizeof(struct memory_chunk_t) + 8);
            }
            cur = cur->next;
        }


        void *temp = custom_sbrk(0);
        void *p = custom_sbrk((intptr_t) (sizeof(struct memory_chunk_t) + 8 + (intptr_t) size + 8));
        if (p != temp) {
            return NULL;
        }
        struct memory_chunk_t *head = (struct memory_chunk_t *) p;

        head->next = NULL;
        head->size = size;
        head->prev = cur;
        cur->next = head;


        char *front_fence = (char *) p + sizeof(struct memory_chunk_t);

        for (int i = 0; i < 8; ++i) {
            *(front_fence + i) = '#';
        }

        char *back_fence = (char *) p + sizeof(struct memory_chunk_t) + 8 + size;

        for (int i = 0; i < 8; ++i) {
            *(back_fence + i) = '#';
        }

        manager.memory_size += (sizeof(struct memory_chunk_t) + 8 + (intptr_t) size + 8);

        return (void *) ((unsigned char *) p + sizeof(struct memory_chunk_t) + 8);
    }

}

void *heap_calloc(size_t number, size_t size) {
    if (size <= 0 || number <= 0) {
        return NULL;
    }
    int total = size * number;
    if (manager.first_memory_chunk == NULL) {
        void *temp = custom_sbrk(0);
        void *p = custom_sbrk((intptr_t) (sizeof(struct memory_chunk_t) + 8 + (intptr_t) total + 8));
        if (p != temp) {
            return NULL;
        }
        struct memory_chunk_t *head = (struct memory_chunk_t *) p;

        head->next = NULL;
        head->size = total;
        head->prev = NULL;
        manager.first_memory_chunk = head;

        char *front_fence = (char *) p + sizeof(struct memory_chunk_t);

        for (int i = 0; i < 8; ++i) {
            *(front_fence + i) = '#';
        }

        char *back_fence = (char *) p + sizeof(struct memory_chunk_t) + 8 + total;

        for (int i = 0; i < 8; ++i) {
            *(back_fence + i) = '#';
        }

        manager.memory_size += (sizeof(struct memory_chunk_t) + 8 + (intptr_t) total + 8);

        char *beg = (char *) ((unsigned char *) p + sizeof(struct memory_chunk_t) + 8);

        for (int i = 0; i < total; ++i) {
            *(beg + i) = 0;
        }
        return (void *) beg;
    } else {
        struct memory_chunk_t *cur = manager.first_memory_chunk;
        while (cur->next != NULL) {
            int max_space = (int) ((char *) cur->next - (char *) cur - sizeof(struct memory_chunk_t) - 8 - cur->size -
                                   8);
            if (max_space >= (int) (sizeof(struct memory_chunk_t) + 8 + total + 8)) {
                struct memory_chunk_t *new = (struct memory_chunk_t *) ((char *) cur + sizeof(struct memory_chunk_t) +
                                                                        8 + cur->size + 8);
                new->prev = cur;
                new->next = cur->next;
                cur->next->prev = new;
                cur->next = new;
                new->size = total;
                char *front_fence = (char *) ((char *) new + sizeof(struct memory_chunk_t));
                for (int i = 0; i < 8; ++i) {
                    *(front_fence + i) = '#';
                }
                char *back_fence = (char *) ((char *) new + sizeof(struct memory_chunk_t) + 8 + total);
                for (int i = 0; i < 8; ++i) {
                    *(back_fence + i) = '#';
                }

                char *beg = (char *) ((char *) new + sizeof(struct memory_chunk_t) + 8);

                for (int i = 0; i < total; ++i) {
                    *(beg + i) = 0;
                }
                return (void *) beg;

            }
            cur = cur->next;
        }


        void *temp = custom_sbrk(0);
        void *p = custom_sbrk((intptr_t) (sizeof(struct memory_chunk_t) + 8 + (intptr_t) total + 8));
        if (p != temp) {
            return NULL;
        }
        struct memory_chunk_t *head = (struct memory_chunk_t *) p;

        head->next = NULL;
        head->size = total;
        head->prev = cur;
        cur->next = head;


        char *front_fence = (char *) p + sizeof(struct memory_chunk_t);

        for (int i = 0; i < 8; ++i) {
            *(front_fence + i) = '#';
        }

        char *back_fence = (char *) p + sizeof(struct memory_chunk_t) + 8 + total;

        for (int i = 0; i < 8; ++i) {
            *(back_fence + i) = '#';
        }

        manager.memory_size += (sizeof(struct memory_chunk_t) + 8 + (intptr_t) total + 8);

        char *beg = (char *) ((unsigned char *) p + sizeof(struct memory_chunk_t) + 8);

        for (int i = 0; i < total; ++i) {
            *(beg + i) = 0;
        }
        return (void *) beg;
    }

}

void *heap_realloc(void *memblock, size_t count) {

    if (count == 0) {
        heap_free(memblock);
        return NULL;
    }
    if (heap_validate() != 0) {
        return NULL;
    }


    if (memblock == NULL) {
        return heap_malloc(count);
    }

    if (get_pointer_type(memblock) != pointer_valid) {
        return NULL;
    }

    struct memory_chunk_t *head = (struct memory_chunk_t *) ((char *) memblock - 8 - sizeof(struct memory_chunk_t));
    if (head->size == count) {
        return memblock;
    } else if (head->size > count) {
        head->size = count;
        char *back_fences = (char *) ((char *) head + sizeof(struct memory_chunk_t) + 8 + count);
        for (int i = 0; i < 8; ++i) {
            *(back_fences + i) = '#';
        }
        return (void *) ((char *) head + sizeof(struct memory_chunk_t) + 8);
    } else {
        if (head->next == NULL) {
            if ((char *) ((char *) head + sizeof(struct memory_chunk_t) + 8 + count + 8) >
                (char *) ((char *) manager.memory_start + manager.memory_size)) {

                if (custom_sbrk((int) ((char *) ((char *) head + sizeof(struct memory_chunk_t) + 8 + count + 8) -
                                       (char *) ((char *) manager.memory_start + manager.memory_size))) !=  (char*)manager.memory_start + manager.memory_size) {
                    return NULL;
                }
                manager.memory_size += (int) ((char *) ((char *) head + sizeof(struct memory_chunk_t) + 8 + count + 8) -
                                              (char *) ((char *) manager.memory_start + manager.memory_size));
            }
            head->size = count;
            char *back_fences = (char *) ((char *) head + sizeof(struct memory_chunk_t) + 8 + head->size);
            for (int i = 0; i < 8; ++i) {
                *(back_fences + i) = '#';
            }
            return (void *) ((char *) head + sizeof(struct memory_chunk_t) + 8);

        }
        int max_size = (int) ((char *) head->next - (char *) head - sizeof(struct memory_chunk_t) - 8 - 8);
        if (max_size >= (int) count) {
            head->size = count;
            char *back_fences = (char *) ((char *) head + sizeof(struct memory_chunk_t) + 8 + head->size);
            for (int i = 0; i < 8; ++i) {
                *(back_fences + i) = '#';
            }
            return (void *) ((char *) head + sizeof(struct memory_chunk_t) + 8);
        } else {
            void *new = heap_malloc(count);
            if (new == NULL) {
                return NULL;
            }
            char *new_casted = (char *) new;
            char *prev_casted = (char *) memblock;
            for (int i = 0; i < (int) head->size; ++i) {
                *(new_casted + i) = *(prev_casted + i);
            }
            heap_free(memblock);
            return new;
        }
    }

}

void heap_free(void *memblock) {
    if (memblock == NULL) {
        return;
    }
    char *casted = (char *) memblock;
    struct memory_chunk_t *cur = manager.first_memory_chunk;
    while (cur != NULL) {
        if (casted == ((char *) cur + sizeof(struct memory_chunk_t) + 8)) {
            if (cur->prev == NULL) {
                if (cur->next == NULL) {
                    manager.first_memory_chunk = NULL;
                    return;
                } else {
                    manager.first_memory_chunk = cur->next;
                    cur->next->prev = NULL;
                    return;
                }
            } else {
                if (cur->next == NULL) {
                    cur->prev->next = NULL;
                    return;
                } else {
                    cur->next->prev = cur->prev;
                    cur->prev->next = cur->next;
                    return;
                }
            }
        }
        cur = cur->next;
    }
}


int fences_check(void) {
    struct memory_chunk_t *cur = manager.first_memory_chunk;

    while (cur != NULL) {
        char *front_fences = (char *) ((char *) cur + sizeof(struct memory_chunk_t));
        for (int i = 0; i < 8; ++i) {
            if (*(front_fences + i) != '#') {
                return 1;
            }
        }

        char *back_fences = (char *) ((char *) cur + sizeof(struct memory_chunk_t) + 8 + cur->size);

        for (int i = 0; i < 8; ++i) {
            if (*(back_fences + i) != '#') {
                return 1;
            }
        }
        cur = cur->next;
    }

    return 0;
}

int chunk_check(void) {
    struct memory_chunk_t *cur = manager.first_memory_chunk;
    if (cur == NULL) {
        return 0;
    }

    while (cur != NULL) {
        if ((char *) cur < (char *) manager.memory_start ||
            (char *) cur + sizeof(struct memory_chunk_t) + 8 + cur->size + 8 >
            (char *) manager.memory_start + manager.memory_size) {
            return 1;
        }

        if (cur->next != NULL) {
            if ((char *) cur->next<(char *) manager.memory_start ||
                                   (char *) cur->next + sizeof(struct memory_chunk_t) + 8 + 1 + 8>
                    (
                char *) manager.memory_start + manager.memory_size) {
                return 1;
            }
            if (cur->next->prev != cur) {
                return 1;
            }
        }

        if (cur->prev != NULL) {
            if ((char *) cur->prev<(char *) manager.memory_start ||
                                   (char *) cur->prev + sizeof(struct memory_chunk_t) + 8 + 1 + 8>
                    (
                char *) manager.memory_start + manager.memory_size) {
                return 1;
            }
            if (cur->prev->next != cur) {
                return 1;
            }

        }
        cur = cur->next;
    }
    return 0;
}

int heap_validate(void) {
    if (manager.memory_start == NULL) {
        return 2;
    }

    if (chunk_check() == 1) {
        return 3;
    }

    if (fences_check() == 1) {
        return 1;
    }


    return 0;
}

enum pointer_type_t get_pointer_type(const void *const pointer) {
    if (pointer == NULL) {
        return pointer_null;
    }

    if (heap_validate() != 0) {
        return pointer_heap_corrupted;
    }

    char *casted_pointer = (char *) pointer;

    struct memory_chunk_t *cur = manager.first_memory_chunk;


    while (cur != NULL) {
        if (casted_pointer >= (char *) cur && casted_pointer < ((char *) cur + sizeof(struct memory_chunk_t))) {
            return pointer_control_block;
        } else if (casted_pointer >= ((char *) cur + sizeof(struct memory_chunk_t)) &&
                   casted_pointer < ((char *) cur + sizeof(struct memory_chunk_t) + 8)) {
            return pointer_inside_fences;
        } else if (casted_pointer == ((char *) cur + sizeof(struct memory_chunk_t) + 8)) {
            return pointer_valid;
        } else if (casted_pointer > ((char *) cur + sizeof(struct memory_chunk_t) + 8) &&
                   casted_pointer < ((char *) cur + sizeof(struct memory_chunk_t) + 8 + cur->size)) {
            return pointer_inside_data_block;
        } else if (casted_pointer >= ((char *) cur + sizeof(struct memory_chunk_t) + 8 + cur->size) &&
                   casted_pointer < ((char *) cur + sizeof(struct memory_chunk_t) + 8 + cur->size + 8)) {
            return pointer_inside_fences;
        } else if (casted_pointer >= ((char *) cur + sizeof(struct memory_chunk_t) + 8 + cur->size + 8) &&
                   casted_pointer < ((char *) cur->next)) {
            return pointer_unallocated;
        }
        cur = cur->next;
    }
    return pointer_unallocated;
}

size_t heap_get_largest_used_block_size(void) {
    if (manager.memory_size == 0 || heap_validate() != 0) {
        return 0;
    }
    struct memory_chunk_t *cur = manager.first_memory_chunk;
    size_t max = 0;
    while (cur != NULL) {
        if (cur->size > max) {
            max = cur->size;
        }
        cur = cur->next;
    }
    return max;
}

