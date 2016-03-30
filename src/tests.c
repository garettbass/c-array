#include <stdio.h>
#include <array.h>


#ifndef assert
    void exit(int);
    static inline
    void error(const char* file, const int line, const char* msg) {
        printf("%s:%i: %s\n", file, line, msg);
        exit(1);
    }
    #define assert(expr) \
        (((expr) ? 0 : error(__FILE__, __LINE__, "assert("#expr") failed")))
#endif


static size_t destructed_element_count = 0;


void destructed_element_count_destructor(int* begin, int* end) {
    for(; begin < end; ++begin) {
        destructed_element_count += 1;
    }
}


typedef array_t(int) int_array;


int main(int argc, const char* argv[]) {
    int_array a = {0};
    assert(array_size(a) == 0);
    assert(array_capacity(a) == 0);

    array_alloc(a, 0, destructed_element_count_destructor);
    assert(array_size(a) == 0);
    assert(array_capacity(a) == 0);


    int* a0 = &array_append(a); *a0 = 1;
    assert(array_size(a) == 1);
    assert(array_capacity(a) >= 1);
    assert(array(a)[0] == 1);


    array_append(a) = 2;
    assert(array_size(a) == 2);
    assert(array_capacity(a) >= 2);
    assert(array(a)[0] == 1);
    assert(array(a)[1] == 2);


    array_append(a) = 3;
    assert(array_size(a) == 3);
    assert(array_capacity(a) >= 3);
    assert(array(a)[0] == 1);
    assert(array(a)[1] == 2);
    assert(array(a)[2] == 3);


    array_insert(a,0) = 0;
    assert(array_size(a) == 4);
    assert(array_capacity(a) >= 4);
    assert(array(a)[0] == 0);
    assert(array(a)[1] == 1);
    assert(array(a)[2] == 2);
    assert(array(a)[3] == 3);


    array_reserve(a,16);
    assert(array_size(a) == 4);
    assert(array_capacity(a) == 16);
    assert(array(a)[0] == 0);
    assert(array(a)[1] == 1);
    assert(array(a)[2] == 2);
    assert(array(a)[3] == 3);


    array_shrink(a);
    assert(array_size(a) == 4);
    assert(array_capacity(a) == 4);
    assert(array(a)[0] == 0);
    assert(array(a)[1] == 1);
    assert(array(a)[2] == 2);
    assert(array(a)[3] == 3);


    array_remove(a,0);
    assert(array_size(a) == 3);
    assert(array_capacity(a) == 4);
    assert(array(a)[0] == 1);
    assert(array(a)[1] == 2);
    assert(array(a)[2] == 3);
    assert(destructed_element_count == 1);
    destructed_element_count = 0;


    array_remove_unordered(a,0);
    assert(array_size(a) == 2);
    assert(array_capacity(a) == 4);
    assert(array(a)[0] == 3);
    assert(array(a)[1] == 2);
    assert(destructed_element_count == 1);
    destructed_element_count = 0;


    array_clear(a);
    assert(array_size(a) == 0);
    assert(array_capacity(a) >= 0);
    assert(destructed_element_count == 2);
    destructed_element_count = 0;


    array_free(a);
    assert(array(a) == NULL);
    assert(array_size(a) == 0);
    assert(array_capacity(a) == 0);


    enum { TEST_LENGTH = 1024 };


    array_alloc(a, 0, destructed_element_count_destructor);
    for (int i = 0; i < TEST_LENGTH; ++i) {
        array_append(a) = i;
    }
    assert(array_size(a) == TEST_LENGTH);
    assert(array_capacity(a) >= TEST_LENGTH);
    for (int i = 0; i < TEST_LENGTH; ++i) {
        assert(array(a)[i] == i);
    }
    {
        int i = 0;
        const int* const end = array_end(a);
        for (int* itr = array_begin(a); itr < end; ++itr) {
            assert(*itr == i++);
        }
    }
    {
        int i = 0;
        while (array_size(a)) {
            assert(array(a)[0] == i++);
            array_remove(a,0);
        }
        assert(array_size(a) == 0);
        assert(array_capacity(a) >= TEST_LENGTH);
        assert(destructed_element_count == TEST_LENGTH);
        destructed_element_count = 0;
    }
    array_free(a);
    assert(array(a) == NULL);
    assert(array_size(a) == 0);
    assert(array_capacity(a) == 0);


    array_alloc(a, 0, destructed_element_count_destructor);
    for (int i = 0; i < TEST_LENGTH; ++i) {
        array_insert(a,0) = i;
    }
    assert(array_size(a) == TEST_LENGTH);
    assert(array_capacity(a) >= TEST_LENGTH);
    for (int i = 0; i < TEST_LENGTH; ++i) {
        assert(array(a)[i] == (TEST_LENGTH - 1) - i);
    }
    array_free(a);
    assert(array(a) == NULL);
    assert(array_size(a) == 0);
    assert(array_capacity(a) == 0);


    puts("array.h: passed tests");
    return 0;
}