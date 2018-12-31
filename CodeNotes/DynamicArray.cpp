#include "DynamicArray.h"

using namespace Dynamic;

/** Ctor functions */
template<class T>
Array<T>::Array(const size_t size)
	: array(size) {}

template<class T>
Array<T>::Array(const Array<T>& rhs)
	: array(rhs.getRaw()) {}

template<class T>
Array<T>& Array<T>::operator=(const Array<T>& rhs) {
	// swap the storage
	array = rhs.array;
	return *this;
}

template<class T>
size_t Array<T>::append(const T& elem) {
	// if we need to expand our array, do so
	if (array.size() + 1 > array.allocated()) array = Storage(array, tH(array.allocated()));
	return array.append(elem);
}

template<class T>
size_t Array<T>::extend(const Array<T>& extendRay) {
	// sanity check
	if (extendRay.size() == 0) return length;
	// check the length of both the arrays, and resize if needed
	if (array.size() + extendRay.size() > array.allocated()) array = Storage(array, tH(array.size() + extendRay.size()));
	// copy the arrays
	array.append(extendRay.getRaw());
	// return the new length
	return array.;
}

template<class T>
size_t Array<T>::extend(const T extendRay[]) {
	size_t rayLength = sizeof(extendRay) / sizeof(T);
	// check the length of both the arrays, and resize if needed
	if (length + rayLength > capacity) resize_capacity(tH(length + rayLength));
	// sanity check
	assert(length + rayLength <= capacity);
	// copy the arrays
	safecopy(&array[length], extendRay, rayLength);
	// set length
	length = length + rayLength;
	// return the new length
	return length;
}

template<class T>
size_t Array<T>::insert(const size_t index, const T& elem) {
	const T ray[] = { elem };
	return insert(index, ray);
}

template<class T>
size_t Array<T>::insert(const size_t index, const T insertRay[]) {
	// sanity checks
	assert(index < length);
	assert(sizeof(insertRay));
	// calculate size based on T
	const size_t insertCount = sizeof(insertRay) / sizeof(T);
	// check if array needs to be expanded
	size_t newCap = capacity;
	if (capacity < length + insertCount) newCap = tH(length + insertCount);
	std::unique_ptr<T[]> newRay = alloc(newCap);
	// copy elements to index
	safecopy(newRay.get(), array, index);
	// copy the insertion array
	safecopy(&newRay.get()[index], insertRay, insertCount);
	// copy the elements after
	safecopy(&newRay.get()[index + insertCount], &array[index], length - index);
	// swap!
	size_t oldLength = length;
	T* oldRay = array;
	array = newRay.release();
	length += insertCount;
	capacity = newCap;
	// dealloc
	dealloc(oldRay, oldLength);
	// return!
	return length;
}

//NOTE: not exception safe!
template<class T>
size_t Array<T>::insert(const size_t index, const Array<T>& insertRay) {
	// sanity checks
	assert(index < length);
	// check if array needs to be expanded
	size_t newCap = capacity;
	if (capacity < length + insertRay.size()) newCap = tH(length + insertRay.size());
	std::unique_ptr<T[]> newRay = alloc(newCap);
	// copy elements to index
	safecopy(newRay.get(), array, index);
	// copy the insertion array
	safecopy(&newRay.get()[index], insertRay.array, insertCount);
	// copy the elements after
	safecopy(&newRay.get()[index + insertRay.size()], &array[index], length - index);
	// swap!
	size_t oldLength = length;
	T* oldRay = array;
	array = newRay.release();
	length += insertCount;
	capacity = newCap;
	// dealloc
	dealloc(oldRay, oldLength);
	// return!
	return length;
}

// NOTE: not exception safe, because I don't want to copy the entire array every time
template<class T>
size_t Array<T>::remove(const size_t index) {
	// sanity check
	assert(index < length);
	std::unique_ptr<T[]> newRay = alloc(capacity);
	// copy elements to index
	safecopy(newRay.get(), array, index);
	// copy the elements after
	safecopy(&newRay.get()[index], &array[index + 1], length - index - 1);
	// swap!
	size_t oldLength = length;
	T* oldRay = array;
	array = newRay.release();
	length--;
	// dealloc
	dealloc(oldRay, oldLength);
	// return!
	return length;
}


template<class T>
inline T Array<T>::pop() {
	return pop(length - 1);
}

template<class T>
T Array<T>::pop(size_t index) {
	// sanity check
	assert(index < length);
	// create a copy of the object to return after deletion
	T hold(ray[index]);
	// delete
	remove(index);
	// return the held element
	return hold;
}

template<class T>
inline void Array<T>::clear() {
	// call the dtor of every element in our array
	for (size_t i = 0; i < length; i++) ray[i].~T();
	// set length to 0
	length = 0;
}

template<class T>
inline int Array<T>::index(const T& elem, const size_t start = 0) const {
	return index(elem, start, length);
}

template<class T>
inline int Array<T>::index(const T& elem, const size_t start, const size_t end) const {
	// sanity checks
	assert(start < end);
	assert(start < length && end <= length);
	assert(length < std::numeric_limits<int>::max());
	// iterate and check equality!
	for (size_t i = start; i < end; i++) if (array[i] == elem) return i;
	// guess not...
	return -1;
}

template<class T>
inline size_t Array<T>::count(const T& elem, const size_t start = 0) const {
	return count(elem, start, length);
}

template<class T>
inline size_t Array<T>::count(const T& elem, const size_t start, const size_t end) const {
	// sanity checks!
	assert(start < end);
	assert(start < length && end <= length);
	// iterate and count!
	size_t count = 0;
	for (size_t i = start; i < end; i++) if (array[i] == elem) count++;
	return count;
}

template<class T>
inline size_t Array<T>::resize(const size_t end) {
	return slice(0, end);
}

template<class T>
size_t Array<T>::resize(const size_t start, const size_t end) {
	// sanity check
	assert(start < end);
	assert(start < length);
	assert(end <= length);
	// allocate a new buffer 
	size_t newCapacity = tH(end - start);
	std::unique_ptr<T[]> newRay = alloc(newCapacity);
	// copy the chosen elements from our old array to the new array
	safecopy(newRay.get(), &array[start], end - start);
	T* oldRay = array;
	size_t oldLength = length;
	// and copy!
	array = newRay.release();
	// and fix that length
	length = end - start;
	capacity = newCapacity;
	// destruct the old array
	dealloc(oldRay, oldLength);
	return length;
}

template<class T>
template<typename I>
inline I Array<T>::tH(const I num) {
	// catch integer overflow for type
	assert(num < std::numeric_limits<I>::max() / 3 - 1);
	return ((num * 3) >> 1) + 1;
}

template<class T>
inline std::unique_ptr<T[]> Array<T>::alloc(const size_t size) {
	return std::unique_ptr<T[]>(reinterpret_cast<T*>(new char[sizeof(T)*size]));
}

template<class T>
inline void Array<T>::dealloc(T* ray, const size_t length) {
	// call dtors to length
	// call the dtor of every element in our array
	for (size_t i = 0; i < length; i++) ray[i].~T();
	// deallocate our array
	delete[] reinterpret_cast<char*>(ray);
}

template<class T>
inline void Array<T>::safecopy(T* copyto, const T* copyfrom, const size_t& length) {
	// use placement new to call every copy constructor safely
	for (size_t i = 0; i < length; i++) new (&copyto[i]) T(copyfrom[i]);
}

/** our most important function */
template<class T>
inline size_t Array<T>::resize_capacity(const size_t newCapacity) {
	// sanity check (only grow the array)
	assert(newCapacity > capacity);
	assert(newCapacity < 1000);
	// create a new buffer
	std::unique_ptr<T[]> newRay = alloc(newCapacity);
	// copy the old buffer into the new one
	safecopy(newRay.get(), array, length);
	// replace the buffer
	T* oldRay = array;
	array = newRay.release();
	// replace the capacity
	capacity = newCapacity;
	// delete the old buffer
	dealloc(oldRay, length);
	// return new capacity
	return capacity;
}