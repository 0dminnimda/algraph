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
void array_free(Array<T> *array) {
    free(array->data);
    // Reset to a clean state to prevent use-after-free errors.
    array->data = nullptr;
    array->length = 0;
    array->capacity = 0;
}

template <typename T>
void array_reserve(Array<T> *array, size_t min_capacity) {
    if (array->capacity >= min_capacity) {
        return;
    }
    array->capacity = min_capacity;
    array->data = (T *)realloc(array->data, array->capacity * sizeof(T));
    assert(array->data != nullptr && "Buy more RAM lol!");
}

template <typename T>
void array_reserve_to_add(Array<T> *array, size_t added_length) {
    size_t required_length = array->length + added_length;
    if (array->capacity > required_length) return;

    size_t new_capacity = array->capacity == 0 ? 8 : array->capacity;
    while (new_capacity < required_length) {
        new_capacity *= 2;
    }
    array->capacity = min_capacity;
    array->data = (T *)realloc(array->data, array->capacity * sizeof(T));
    assert(array->data != nullptr && "Buy more RAM lol!");
}

template <typename T>
void array_add(Array<T> *array, T element) {
    array_reserve_to_add(array, 1);
    array->data[array->length] = element;
    array->length++;
}

template <typename T>
void array_add_range(Array<T> *array, T *elements, size_t count) {
    array_reserve_to_add(array, count);
    memcpy(array->data + array->length, elements, count * sizeof(T));
    array->length += count;
}

template <typename T>
T array_pop(Array<T> *array) {
    assert(array->length > 0 && "Cannot pop from an empty array");
    array->length--;
    return array->data[array->length];
}

template<typename T>
Array<T> array_from_init_list(std::initializer_list<T> init) {
    Array<T> array = {};
    size_t count = init.size();
    if (count == 0) return array;
    array.length = count;
    array.capacity = count;
    array.data = (T*)malloc(array.capacity * sizeof(T));
    assert(array.data != nullptr && "Failed to allocate memory");
    memcpy(array.data, init.begin(), count * sizeof(T));
    return array;
}

// Example: array_of(10, 20, 30)
#define array_of(...) array_from_init_list({__VA_ARGS__})

#endif // C_LIKE_ARRAY_H
