#ifndef C_LIKE_ARRAY_H
#define C_LIKE_ARRAY_H

#include <stddef.h> // for size_t
#include <stdlib.h> // for realloc, free
#include <assert.h> // for assert
#include <string.h> // for memcpy
#include <initializer_list>


template <typename T>
struct Array {
    T *data = nullptr;
    size_t length = 0;
    size_t capacity = 0;

    T &operator[](size_t index) {
        assert(index < length);
        return data[index];
    }
};

template <typename T>
T *begin(Array<T>& arr) { return arr.data; }

template <typename T>
T *end(Array<T>& arr) { return arr.data + arr.length; }

template <typename T>
void array_free(Array<T> *arr) {
    free(arr->data);
    // Reset to a clean state to prevent use-after-free errors.
    arr->data = nullptr;
    arr->length = 0;
    arr->capacity = 0;
}

template <typename T>
void array_reserve(Array<T> *arr, size_t min_capacity) {
    if (arr->capacity >= min_capacity) {
        return;
    }
    arr->capacity = min_capacity;
    arr->data = (T *)realloc(arr->data, arr->capacity * sizeof(T));
    assert(arr->data != nullptr && "Buy more RAM lol!");
}

template <typename T>
void array_push(Array<T> *arr, T element) {
    if (arr->length >= arr->capacity) {
        size_t new_capacity = arr->capacity == 0 ? 8 : arr->capacity * 2;
        array_reserve(arr, new_capacity);
    }
    arr->data[arr->length] = element;
    arr->length++;
}

template <typename T>
T array_pop(Array<T> *arr) {
    assert(arr->length > 0 && "Cannot pop from an empty array");
    arr->length--;
    return arr->data[arr->length];
}

template<typename T>
Array<T> array_from_init_list(std::initializer_list<T> init) {
    Array<T> arr = {};
    size_t count = init.size();
    if (count == 0) return arr;
    arr.length = count;
    arr.capacity = count;
    arr.data = (T*)malloc(arr.capacity * sizeof(T));
    assert(arr.data != nullptr && "Failed to allocate memory");
    memcpy(arr.data, init.begin(), count * sizeof(T));
    return arr;
}

// Example: array_of(10, 20, 30)
#define array_of(...) array_from_init_list({__VA_ARGS__})

#endif // C_LIKE_ARRAY_H
