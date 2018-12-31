#include "DynamicStorage.h"

using namespace Dynamic;

template<class T>
Storage<T>::Storage(const size_t size = 0)
	: capacity(size)
	, length(0)
	, arrayPtr(alloc(size)) {
	// sanity check
	assert(capacity < 1000);
}

template<class T>
Storage<T>::Storage(const Storage<T>& copyFrom, const size_t size)
	: capacity(size)
	, length(0)
	, arrayPtr(alloc(capacity)) {
	// sanity!
	assert(capacity >= copyFrom.allocated());
	assert(capacity < 1000);
	assert(copyFrom.size() <= capacity);
	// copy the elements from copyFrom to our array
	append(copyFrom.getRaw(), copyFrom.size());
}

template<class T>
Storage<T>::Storage(const Storage<T>& rhs)
	: capacity(rhs.allocated())
	, length(0)
	, arrayPtr(alloc(capacity)) {
	// sanity!
	assert(capacity < 1000);
	assert(copyFrom.size() <= capacity);
	// copy the elements from copyFrom to our array
	append(copyFrom.getRaw(), copyFrom.size());
}

template<class T>
Storage<T>::~Storage() {
	// deallocate our array!
	dealloc(arrayPtr, length);
	// and deallocate our temporary array, if it exists
	if (tempArrayPtr != nullptr) dealloc(tempArrayPtr, tempLength);
}

template<class T>
Storage<T>& Storage<T>::operator=(const Storage<T>& rhs) {
	// copy and swap!
	// create a copy of rhs's data into the temporary buffers
	tempLength = 0;
	tempArrayPtr = alloc(rhs.allocated());
	for (; tempLength < rhs.size(); tempLength++) new (&tempArrayPtr[tempLength]) T(rhs[tempLength]);
	// swap the our array with the copied array
	std::swap(arrayPtr, tempArrayPtr);
	std::swap(length, tempLength);
	capacity = rhs.allocated();
	// deallocate the old buffer
	dealloc(tempArrayPtr, tempLength);
	// and back to zeros with our temp varibles
	tempArrayPtr = nullptr;
	tempLength = 0;
}

template<class T>
const T& Storage<T>::operator[](const size_t index) const {
	assert(index < length);
	return arrayPtr[index];
}

template<class T>
T& Storage<T>::operator[](const size_t index) {
	assert(index < length);
	return arrayPtr[index];
}

template<class T>
size_t Storage<T>::append(const T elems[], const size_t count, const size_t start = 0) {
	// sanity check
	assert(length + count <= capacity);
	// start copying using placement new, incrementing the length as we go!
	for (size_t i = 0; i < count; length++, i++) new (&arrayPtr[length]) T(elems[i]);
	// return the new length
	return length;
}

template<class T>
size_t Storage<T>::truncate(size_t count = 1) {
	assert(count >= length);
	// decrement length, and call dtor of the removed object
	for (; count > 0; --length, count--) arrayPtr[length].~T();
	// return the new length
	return length;
}

template<class T>
inline T* Storage<T>::alloc(const size_t size) {
	return reinterpret_cast<T*>(new char[sizeof(T)*size]);
}

template<class T>
inline void Storage<T>::dealloc(T* ray, const size_t size) {
	// call the dtors of all the elements
	for (size_t i = 0; i < size; i++) ray[i].~T();
	// deallocate the array
	delete[] reinterpret_cast<char*>(ray);
}