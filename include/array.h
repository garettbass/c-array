/*
Copyright (c) 2016 Garett Bass (https://github.com/garettbass)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once
#include <stddef.h>


#if __cplusplus
extern "C" {
#endif // __cplusplus


#ifndef array_allocator
    static inline void* array_allocator(void* ptr, size_t size) {
        #ifndef realloc
            extern void* realloc(void* ptr, size_t size);
        #endif
        #ifndef free
            extern void free(void* ptr);
        #endif
        return size ? (realloc(ptr, size)) : (free(ptr),NULL);
    }
#endif


//------------------------------------------------------------------------------


// NOTE: this could just be T*, but that would invite accidental misuse
#define array_t(T) union { _array_t _array; T* array; }


// T* array(array_t(T)& array)
#define array(a) ((a).array)


// void array_alloc(array_t(T)& array, size_t capacity, void (*destructor)(T* begin, T* end))
#define array_alloc(a, capacity, destructor) \
    (_array_alloc(_array_ref(a), capacity, array_allocator, (_array_destructor_t)destructor))


// void array_free(array_t(T)& array)
#define array_free(a) \
    (_array_free(_array_ref(a)))


// void array_reserve(array_t(T)& array, size_t capacity)
#define array_reserve(a, capacity) \
    (_array_reserve(_array_ref(a), (capacity) * _array_stride(a)))


// void array_resize(array_t(T)& array, size_t size)
#define array_resize(a, size) \
    (_array_resize(_array_ref(a), (size) * _array_stride(a)))


// void array_shrink(array_t(T)& array)
#define array_shrink(a) \
    (_array_shrink(_array_ref(a)))


// size_t array_capacity(const T* array)
#define array_capacity(a) \
    (_array_capacity(_array_ref(a)) / _array_stride(a))


// int array_compare(const T* array_a, const T* array_b)
#define array_compare(a, b) \
    (_array_compare(_array_ref(a), _array_ref(b)))


// size_t array_size(const T* array)
#define array_size(a) \
    (_array_size(_array_ref(a)) / _array_stride(a))


// bool array_empty(const T* array)
#define array_empty(a) \
    ((bool)(_array_size(_array_ref(a)) == 0))


// T& array_front(T* array)
#define array_front(a) \
    ((a).array[ _array_front_index(_array_ref(a)) ])


// T& array_back(T* array)
#define array_back(a) \
    ((a).array[ _array_back_index(_array_ref(a), _array_stride(a)) ])


// T& array_append(array_t(T)& array)
#define array_append(a) \
    ((a).array[ _array_append(_array_ref(a), _array_stride(a)) / _array_stride(a) ])


// T* array_append_n(array_t(T)& array, size_t count)
#define array_append_n(a, count) \
    ((a).array + _array_append(_array_ref(a), _array_offset(a, (count))) / _array_stride(a))


// T& array_insert(array_t(T)& array, size_t index)
#define array_insert(a, index) \
    ((a).array[ _array_insert(_array_ref(a), _array_offset(a, (index)), _array_stride(a)) / _array_stride(a) ])


// T* array_insert_n(array_t(T)& array, sizet index, size_t count)
#define array_insert_n(a, index, count) \
    ((a).array + _array_insert(_array_ref(a), _array_offset(a, (index)), _array_offset(a, (count))) / _array_stride(a))


// void array_remove(array_t(T)& array, size_t index)
#define array_remove(a, index) \
    (_array_remove(_array_ref(a), _array_offset(a, (index)), _array_stride(a)))


// void array_remove_unordered(array_t(T)& array, size_t index)
#define array_remove_unordered(a, index) \
    (_array_remove_unordered(_array_ref(a), _array_offset(a, (index)), _array_stride(a)))


// void array_remove_n(array_t(T)& array, size_t index, size_t count)
#define array_remove_n(a, index, count) \
    (_array_remove(_array_ref(a), _array_offset(a, (index)), _array_offset(a, (count))))


// void array_clear(array_t(T)& array)
#define array_clear(a) \
    (_array_clear(_array_ref(a)))


// T* array_begin(T* array)
#define array_begin(a) ((a).array)


// T* array_end(T* array)
#define array_end(a) ((a).array + (_array_size(_array_ref(a)) / _array_stride(a)))


// T& array_push(array_t(T)& array)
#define array_push(a) array_append(a)


// void array_pop(array_t(T)& array)
#define array_pop(a) array_remove(a, _array_back_index(_array_ref(a)))


// T& array_top(T* array)
#define array_top(a) array_back(a)


//==============================================================================


#define _array_ref(a) (&((a)._array))

#define _array_stride(a) (sizeof((a).array[0]))

#define _array_offset(a, n) ((size_t)(_array_stride(a) * n))


//------------------------------------------------------------------------------


typedef char* _array_t;

typedef void* (*_array_allocator_t)(void* array, size_t size);

typedef void (*_array_destructor_t)(void* begin, void* end);

typedef struct {
    _array_allocator_t allocator;
    _array_destructor_t destructor;
    size_t capacity, size;
    char data[0];
} _array_header_t;


//------------------------------------------------------------------------------


#ifndef _array_assert
    void exit(int);
    int printf(const char*, ...);
    static inline
    void _array_error(const char* file, const int line, const char* msg) {
        printf("%s:%i: %s\n", file, line, msg);
        exit(1);
    }
    #define _array_assert(expr, msg) \
        (((expr) ? 1 : (_array_error(__FILE__, __LINE__, "assert("#expr") failed: "msg), 0)))
#endif


#ifndef _array_memcmp
    #define _array_memcmp memcmp
    #ifndef memcmp
        int memcmp(const void*, const void*, size_t);
    #endif
#endif


#ifndef _array_memcpy
    #define _array_memcpy memcpy
    #ifndef memcpy
        void* memcpy(void*, const void*, size_t);
    #endif
#endif


#ifndef _array_memmove
    #define _array_memmove memmove
    #ifndef memmove
        void* memmove(void*, const void*, size_t);
    #endif
#endif


#ifndef _array_memset
    #define _array_memset memset
    #ifndef memset
        void* memset(void*, int, size_t);
    #endif
#endif


//------------------------------------------------------------------------------


static inline
size_t _array_ceilpow2(size_t x) {
    enum { _32_OR_0 = 32 * (sizeof(void*) > 4) };
    x -= 1;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    x |= (x >> _32_OR_0);
    x += 1;
    return x ? x : ~((size_t)0);
}


static inline
_array_header_t* _array_header(_array_t* const a) {
    _array_header_t* const headers = (_array_header_t*)(*a);
    return headers ? (headers - 1) : headers;
}


static inline
size_t _array_capacity(_array_t* const a) {
    const _array_header_t* const header = _array_header(a);
    return header ? header->capacity : ((size_t)0);
}


static inline
size_t _array_size(_array_t* const a) {
    const _array_header_t* const header = _array_header(a);
    return header ? header->size : ((size_t)0);
}


static inline
void _array_alloc(_array_t* a, const size_t capacity, _array_allocator_t allocator, _array_destructor_t destructor) {
    _array_assert(!(*a), "array already allocated");
    const size_t mem_size = sizeof(_array_header_t) + capacity;
    _array_header_t* const header = 
        (_array_header_t*)allocator(NULL, mem_size);
    header->allocator = allocator;
    header->destructor = destructor;
    header->capacity = capacity;
    header->size = 0;
    (*a) = header->data;
}


static inline
void _array_free(_array_t* a) {
    _array_header_t* header = _array_header(a);
    if (header) {
        if (header->destructor) {
            const size_t free_size = header->size;
            char* free_begin = (*a);
            char* free_end = free_begin + free_size;
            header->destructor(free_begin, free_end);
        }
        header = header->allocator(header, 0);
        _array_assert(header == NULL, "allocator leaked memory");
    }
    (*a) = NULL;
}


static inline
void _array_grow(_array_t* a, const size_t capacity) {
    const size_t mem_size = sizeof(_array_header_t) + capacity;
    _array_header_t* header = _array_header(a);
    header = (_array_header_t*)header->allocator(header, mem_size);
    header->capacity = capacity;
    (*a) = header->data;
}


static inline
void _array_shrink(_array_t* a) {
    _array_header_t* old_header = _array_header(a);
    if (old_header->capacity > old_header->size) {
        const size_t new_capacity = old_header->size;
        const size_t mem_size = sizeof(_array_header_t) + new_capacity;
        const _array_allocator_t allocator = old_header->allocator;
        _array_header_t* new_header = (_array_header_t*)allocator(NULL, mem_size);
        _array_memcpy(new_header, old_header, mem_size);
        old_header = old_header->allocator(old_header, 0);
        _array_assert(old_header == NULL, "allocator leaked memory");
        new_header->capacity = new_capacity;
        (*a) = new_header->data;
    }
}


static inline
void _array_reserve(_array_t* a, const size_t capacity) {
    _array_assert((*a), "array uninitialized");
    if (_array_capacity(a) < capacity) {
        _array_grow(a, _array_ceilpow2(capacity));
    }
}


static inline
void _array_resize(_array_t* a, const size_t new_size) {
    _array_assert((*a), "array uninitialized");
    _array_header_t* header = _array_header(a);
    const size_t old_size = header->size;
    if (old_size > new_size) {
        if (header->destructor) {
            const size_t discard_size = old_size - new_size;
            char* discard_begin = (*a) + new_size;
            char* discard_end = discard_begin + discard_size;
            header->destructor(discard_begin, discard_end);
        }
        header->size = new_size;
        return;
    }
    _array_reserve(a, new_size);
    _array_header(a)->size = new_size;
}


static inline
int _array_compare(_array_t* a, _array_t* b) {
    const size_t size_a = _array_size(a);
    const size_t size_b = _array_size(b);
    const size_t size = (size_a < size_b) ? size_a : size_b;
    const int cmp = _array_memcmp((*a), (*b), size);
    return (cmp) ? cmp : (int)(size_b - size_a);
}


static inline
size_t _array_append(_array_t* a, const size_t append_size) {
    _array_assert((*a), "array uninitialized");
    const size_t append_offset = _array_size(a);
    const size_t new_size = append_offset + append_size;
    _array_reserve(a, new_size);
    _array_header(a)->size = new_size;
    return append_offset;
}


static inline
size_t _array_insert(_array_t* a, const size_t insert_offset, const size_t insert_size) {
    _array_assert((*a), "array uninitialized");
    const size_t old_size = _array_size(a);
    _array_assert(insert_offset <= old_size, "array index out of range");
    const size_t new_size = old_size + insert_size;
    _array_reserve(a, new_size);
    _array_header(a)->size = new_size;
    char* insert_begin = (*a) + insert_offset;
    char* insert_end = insert_begin + insert_size;
    const size_t end_size = old_size - insert_offset;
    _array_memmove(insert_end, insert_begin, end_size);
    return insert_offset;
}


static inline
void _array_remove(_array_t* a, const size_t remove_offset, const size_t remove_size) {
    _array_assert((*a), "array uninitialized");
    const size_t old_size = _array_size(a);
    _array_assert(remove_offset <= old_size, "array index out of range");
    _array_assert(remove_size <= old_size - remove_offset, "array index out of range");
    const size_t new_size = old_size - remove_size;
    _array_header_t* header = _array_header(a);
    char* remove_begin = (*a) + remove_offset;
    char* remove_end = remove_begin + remove_size;
    if (header->destructor) {
        header->destructor(remove_begin, remove_end);
    }
    const size_t tail_size = new_size - remove_offset;
    _array_memmove(remove_begin, remove_end, tail_size);
    header->size = new_size;
}


static inline
void _array_remove_unordered(_array_t* a, const size_t remove_offset, const size_t remove_size) {
    _array_assert((*a), "array uninitialized");
    const size_t old_size = _array_size(a);
    _array_assert(remove_offset <= old_size, "array index out of range");
    _array_assert(remove_size <= old_size - remove_offset, "array index out of range");
    const size_t new_size = old_size - remove_size;
    _array_header_t* header = _array_header(a);
    char* remove_begin = (*a) + remove_offset;
    char* remove_end = remove_begin + remove_size;
    if (header->destructor) {
        header->destructor(remove_begin, remove_end);
    }
    char* tail_begin = (*a) + new_size;
    _array_memmove(remove_begin, tail_begin, remove_size);
    header->size = new_size;
}


static inline
size_t _array_front_index(_array_t* const a) {
    _array_assert((*a), "array uninitialized");
    return 0;
}


static inline
size_t _array_back_index(_array_t* const a, const size_t stride) {
    _array_assert((*a), "array uninitialized");
    const size_t size = _array_size(a);
    _array_assert(size, "array index out of range");
    return (size / stride) - 1;
}


static inline
void _array_clear(_array_t* const a) {
    _array_assert((*a), "array uninitialized");
    _array_header_t* header = _array_header(a);
    const size_t clear_size = header->size;
    char* clear_begin = (*a);
    char* clear_end = clear_begin + clear_size;
    if (header->destructor) {
        header->destructor(clear_begin, clear_end);
    }
    header->size = 0;
}


//------------------------------------------------------------------------------


#if __cplusplus
} // extern "C"
#endif // __cplusplus


#if array_inline
#include "array.inl"
#endif