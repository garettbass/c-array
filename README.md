# c-array
A simple std::vector-style array for C

```C
int* a = NULL;

// allocate array 'a' with capacity of int[16], and no destructor callback
array_alloc(a, 16, NULL);

array_append(a) = 1; // append to the array, and assign 1 to the appended integer

a[0] = 2; // change the value of a[0] from 1 to 2

array_remove(a, 0); // remove integer at index 0

for (int i = 0; i < 10; ++i) {
	array_append(a) = i;
}

// index-based iteration
const size_t a_size = array_size(a);
for (int i = 0; i < a_size; ++i) {
	printf("%i, ", a[i]);
}
puts("\n");


// pointer-based iteration
const int* const end = array_end(a);
for (int* itr = a; itr < end; ++itr) {
	printf("%i, ", *itr);
}
puts("\n");

array_free(a); // free memory
```