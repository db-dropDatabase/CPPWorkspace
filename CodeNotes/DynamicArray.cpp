#include "DynamicArray.h"

/** Ctor functions */
template<class T>
DynamicArray<T>::DynamicArray(const size_t size)
	: length(size)
	, capacity(tH(size))
	// allocate using chars to prevent calling the defualt ctor
	, array(alloc(capacity)) {
	// sanity checks
	assert(length < 1000);
	assert(capacity > length);
	assert(capacity > 0);
}

template<class T>
DynamicArray<T>::DynamicArray(const DynamicArray<T>& rhs)
	: length(rhs.length)
	, capacity(rhs.capacity)
	// allocate using chars to prevent calling the defualt ctor
	, array(alloc(rhs.capacity))
{
	// sanity checks
	assert(length < 1000);
	assert(capacity > length);
	assert(capacity > 0);
	/** copy each element into the array */
	safecopy(array, rhs.array, rhs.length);
}

template<class T>
DynamicArray<T>::~DynamicArray() {
	dealloc(array, length);
}

/** member funtions*/
template<class T>
inline void* DynamicArray<T>::getRaw() {
	return reinterpret_cast<void*>(array);
}

template<class T>
const inline T& DynamicArray<T>::operator[](const size_t index) const {
	assert(index < length);
	return array[index];
}

template<class T>
inline T& DynamicArray<T>::operator[](const size_t index) {
	assert(index < length);
	return array[index];
}

template<class T>
DynamicArray<T>& DynamicArray<T>::operator=(const DynamicArray<T>& rhs) {
	// create a copy of each element and swap
	T* newRay = alloc(rhs.capacity);
	safecopy(newRay, rhs.array, rhs.length);
	// deallocate, and replace
	dealloc(array, length);
	array = newRay;
	// copy the other members
	length = rhs.length;
	capacity = rhs.capacity;
	return *this;
}

template<class T>
size_t DynamicArray<T>::append(const T& elem) {
	// if we need to expand our array, do so
	if (length + 1 > capacity) resize_capacity(tH(length));
	// sanity check
	assert(length + 1 <= capacity);
	// copy the element over using the copy ctor
	new (&array[length]) T(elem);
	// increment the length afterwards for exception saftey
	return ++length;
}

template<class T>
size_t DynamicArray<T>::extend(const DynamicArray<T>& extendRay) {
	// sanity check
	if (extendRay.size() == 0) return length;
	// check the length of both the arrays, and resize if needed
	if (length + extendRay.size() > capacity) resize_capacity(tH(length + extendRay.size()));
	// more sanity check
	assert(length + extendRay.size() <= capacity);
	// copy the arrays
	safecopy(&array[length], extendRay.array, extendRay.size());
	// set length
	length = length + extendRay.size();
	// return the new length
	return length;
}

template<class T>
size_t DynamicArray<T>::extend(const T extendRay[]) {
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

// NOTE: not exception safe!
template<class T>
size_t DynamicArray<T>::insert(const size_t index, const T& elem) {
	// sanity check
	assert(index < length);
	// check if the array needs to be expanded
	if (capacity < length + 1) resize_capacity(tH(length + 1));
	// copy elements over starting from the end, and working out way back
	for (size_t i = length - 1; i >= index; i--) {
		// copy the element over using placement new
		new (&array[i + 1]) T(array[i]);
		// destruct the element copied
		array[i].~T();
	}
	// replace index
	new (&array[index]) T(elem);
	// return the new length
	return ++length;
}

//NOTE: not exception safe!
template<class T>
size_t DynamicArray<T>::insert(const size_t index, const T insertRay[]) {
	// sanity checks
	assert(index < length);
	assert(sizeof(insertRay));
	// calculate size based on T
	const size_t insertCount = sizeof(insertRay) / sizeof(T);
	// check if array needs to be expanded
	if (capacity < length + insertCount) resize_capacity(tH(length + insertCount));
	// copy elements starting from the end, and working back
	for (size_t i = length - 1; i >= index; i--) {
		// copy the element over using placement new
		new (&array[i + insertCount]) T(array[i]);
		// destruct the element copied
		array[i].~T();
	}
	// copy the insert array
	for (size_t i = 0; i < insertCount; i++) new (&array[index + i]) T(insertRay[i]);
	// return the new length
	return length += insertCount;
}

//NOTE: not exception safe!
template<class T>
size_t DynamicArray<T>::insert(const size_t index, const DynamicArray<T>& insertRay) {
	// sanity checks
	assert(index < length);
	if (insertRay.size() == 0) return length;
	// check if array needs to be expanded
	if (capacity < length + insertRay.size()) resize_capacity(tH(length + insertRay.size()));
	// copy elements starting from the end, and working back
	for (size_t i = length - 1; i >= index; i--) {
		// copy the element over using placement new
		new (&array[i + insertRay.size()]) T(array[i]);
		// destruct the element copied
		array[i].~T();
	}
	// copy the insert array
	for (size_t i = 0; i < insertRay.size(); i++) new (&array[index + i]) T(insertRay[i]);
	// return the new length
	return length += insertCount;
}

// NOTE: not exception safe, because I don't want to copy the entire array every time
template<class T>
size_t DynamicArray<T>::remove(const size_t index) {
	// sanity check
	assert(index < length);
	// call the dtor of each element being moved over, then copy the next element into the destroyed element
	for (size_t i = index; i < length - 1; i++) {
		ray[i].~T();
		// placement new!
		new (&ray[i]) T(ray[i + 1]);
	}
	// call the dtor of the last element
	ray[length - 1].~T();
	// decrement length and return
	return --length;
}

template<class T>
inline T DynamicArray<T>::pop() {
	return pop(length - 1);
}

template<class T>
T DynamicArray<T>::pop(size_t index) {
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
inline void DynamicArray<T>::clear() {
	// call the dtor of every element in our array
	for (size_t i = 0; i < length; i++) ray[i].~T();
	// set length to 0
	length = 0;
}

template<class T>
inline int DynamicArray<T>::index(const T& elem, const size_t start = 0) const {
	return index(elem, start, length);
}

template<class T>
inline int DynamicArray<T>::index(const T& elem, const size_t start, const size_t end) const {
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
inline size_t DynamicArray<T>::count(const T& elem, const size_t start = 0) const {
	return count(elem, start, length);
}

template<class T>
inline size_t DynamicArray<T>::count(const T& elem, const size_t start, const size_t end) const {
	// sanity checks!
	assert(start < end);
	assert(start < length && end <= length);
	// iterate and count!
	size_t count = 0;
	for (size_t i = start; i < end; i++) if (array[i] == elem) count++;
	return count;
}

template<class T>
inline size_t DynamicArray<T>::resize(const size_t end) {
	return slice(0, end);
}

template<class T>
size_t DynamicArray<T>::resize(const size_t start, const size_t end) {
	// sanity check
	assert(start < end);
	assert(start < length);
	assert(end <= length);
	// allocate a new buffer 
	size_t newCapacity = tH(end - start);
	T* newRay = alloc(newCapacity);
	// copy the chosen elements from our old array to the new array
	safecopy(newRay, &array[start], end - start);
	// destruct the old array
	dealloc(array, length);
	// and copy!
	array = newRay;
	// and fix that length
	length = end - start;
	capacity = newCapacity;
	return length;
}

template<class T>
inline size_t DynamicArray<T>::size() const {
	return length;
}

template<class T>
inline size_t DynamicArray<T>::allocatedSize() const {
	return capacity;
}

template<class T>
template<typename I>
inline I DynamicArray<T>::tH(const I num) {
	// catch integer overflow for type
	assert(num < std::numeric_limits<I>::max() / 3 - 1);
	return ((num * 3) >> 1) + 1;
}

template<class T>
inline T* DynamicArray<T>::alloc(const size_t& size) {
	return reinterpret_cast<T*>(new char[sizeof(T)*size]);
}

template<class T>
inline void DynamicArray<T>::dealloc(T*& ray, const size_t& length) {
	// call dtors to length
	// call the dtor of every element in our array
	for (size_t i = 0; i < length; i++) ray[i].~T();
	// deallocate our array
	delete[] reinterpret_cast<char*>(ray);
}

template<class T>
inline void DynamicArray<T>::safecopy(T* copyto, const T* copyfrom, const size_t& length) {
	// use placement new to call every copy constructor safely
	for (size_t i = 0; i < length; i++) new (&copyto[i]) T(copyfrom[i]);
}

/** our most important function */
template<class T>
inline size_t DynamicArray<T>::resize_capacity(const size_t newCapacity) {
	// sanity check (only grow the array)
	assert(newCapacity > capacity);
	assert(newCapacity < 1000);
	// create a new buffer
	T* newRay = alloc(newCapacity);
	// copy the old buffer into the new one
	safecopy(newRay, array, length);
	// delete the old buffer
	dealloc(array, length);
	// replace the buffer
	array = newRay;
	// replace the capacity
	capacity = newCapacity;
	return capacity;
}