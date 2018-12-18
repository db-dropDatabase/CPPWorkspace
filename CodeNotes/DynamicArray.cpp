/**
 * One of the more annoying requirements of CS162 is forcing the use of dynamically allocated arrays instead of vectors,
 * which, though probably faster, results in a lot of headaches and memeory leaks. As a result, I've decided to make my 
 * own implementatiom of a dynamic array {@link https://en.wikipedia.org/wiki/Dynamic_array}, which I will use in future labs.
 * This implementation will hopefully use the features of modern C++ to make the use of dynamic memory simple, but also
 * uphold the archaic requirements of CS162.
 * 
 * CS162 Rules of engagement:
 *  * No standard library containers (vector, list, etc.)
 *  * No smart pointers
 *  * I kinda have to use template functions if I want to make this work, but I'm not quite sure if I'm supposed to
 * 
 * I have heavily relied on Effective C++ third edition and
 * https://codereview.stackexchange.com/questions/60484/stl-vector-implementation
 * for my knowlege on C++.
 */

#include <assert.h>

template<class T>
class DynamicArray {
    public:
        /**
         * Constructor
         * @param length the size of array to allocate at the start
         */
        explicit DynamicArray(const size_t size = 0);
        
        /** Copy Constructor, shallow copies the entire array */
        DynamicArray(const DynamicArray<T>& rhs);

        /** Dtor */
        ~DynamicArray();

        /** Get the dumb array pointer */
        char* getRaw();

        /** Override the array bracket operator */
        const T& operator[](const size_t index) const;
        T& operator[](const size_t index);

        /** Override the = operator and do an in-place copy */
        DynamicArray<T>& operator=(const DynamicArray<T>& rhs);

        /** 
         * allow pushing back elements to the array 
         * @param T& elem teh element to add to the array
         * NOTE: By adding the element, please ensure the element is only accesible from the DynamicArray, as the DynamicArray may
         * decide to destroy the element before you're done with it otherwise.
         */
        size_t push(const T& elem);

        /** reduce the size of the vector by one (on off the end) */
        size_t pop();

        /** 
         * Extend this DynamicArray with another array's data
         * @param ray The dynamic array to pull data from
         * @param length The length of the array, if it's a static array
         */
        size_t extend(const DynamicArray<T>& ray);
        size_t extend(const T ray[]);

        /**
         * Resize the array based on a start and end offset,
         * similair to slice() in other languages except it works in place
         * @param start the index to start the new array, inclusive
         * @param end the index to end the new array, exclusive
         */
		size_t slice(const size_t& end);
		size_t slice(const size_t& start, const size_t& end);

        size_t size() const;
        size_t allocatedSize() const;
    private:
        /** 
         * utility function to do a cheap 3/2, always rounding up.
         * NOTE: will always return a number > or < 0
         * @param number the number to multiply by 1.5
         */
        template<typename I>
        static I tH(const I num);

		/**
		 * Utility function to allocate memory, so I don't
		 * have to reuse code
		 * @param size the size of the memory to allocate, in units of T
		 */
		static T* alloc(const size_t& size);

		/**
		 * Utility function to dellocate an array with a length and capacity,
		 * making sure to only call the needed destructors and remove the memory
		 * @param ray the array to deallocate
		 * @param length the number of objects that have been enstanciated in the array (not the capacity)
		 */
		static void dealloc(T*& ray, const size_t& length);

		/**
		 * Ultility function to safely copy an array of objects using the placement new operator
		 * @param copyto the object to copy to
		 * @param copyfrom the object to copy the data from
		 * @param length the number of objects of type T to copy
		 */
		static void safecopy(T* copyto, const T* copyfrom, const size_t& length);

        /** 
         * resize the capacity of the array 
         * @param newCapacity the size of the new array, in units of T (must be larger than the initial capacity)
         */
        size_t resize_capacity(const size_t newCapacity);

        unsigned int length;
        unsigned int capacity;
        T* array;
};

/** member functions */
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

template<class T>
char* DynamicArray<T>::getRaw() {
    return reinterpret_cast<char*>(array);
}

template<class T>
const T& DynamicArray<T>::operator[](const size_t index) const {
    assert(index < length);
    return array[index];
}

template<class T>
T& DynamicArray<T>::operator[](const size_t index) {
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
size_t DynamicArray<T>::push(const T& elem) {
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
size_t DynamicArray<T>::pop() {
    // sanity check
    assert(length > 0);
    // call the dtor of the object we're deleting
    array[length - 1].~T();
    // decrement length
    return --length;
}

template<class T>
size_t DynamicArray<T>::extend(const DynamicArray<T>& ray) {
    // sanity check
    if (ray.size() == 0) return length;
    // check the length of both the arrays, and resize if needed
    if (length + ray.size() > capacity) resize_capacity(tH(length + ray.size()));
    // more sanity check
    assert(length + ray.size() <= capacity);
	// copy the arrays
	safecopy(&array[length], ray.array, ray.size());
	// set length
	length = length + ray.size();
    // return the new length
    return length;
}

template<class T>
size_t DynamicArray<T>::extend(const T ray[]) {
    size_t rayLength = sizeof(ray)/sizeof(T);
    // check the length of both the arrays, and resize if needed
    if (length + rayLength > capacity) resize_capacity(tH(length + rayLength))
    // sanity check
    assert(length + rayLength <= capacity);
    // copy the arrays
	safecopy(&array[length], ray, rayLength);
	// set length
	length = length + rayLength;
    // return the new length
    return length;
}

template<class T>
inline size_t DynamicArray<T>::slice(const size_t& end) {
	return slice(0, end);
}

template<class T>
size_t DynamicArray<T>::slice(const size_t& start, const size_t& end) {
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