#pragma once
/**
 * One of the more annoying requirements of CS162 is forcing the use of dynamically allocated arrays instead of vectors,
 * which, though probably faster, results in a lot of headaches and memeory leaks. As a result, I've decided to make my 
 * own implementatiom of a dynamic array {@link https://en.wikipedia.org/wiki/Dynamic_array} based on the Python list implementation,
 * {@link https://docs.python.org/3/tutorial/datastructures.html} which I will use in future labs. This implementation 
 * will hopefully use the features of modern C++ to make the use of dynamic memory simple, but also uphold the archaic 
 * requirements of CS162.
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

#include "DynamicStorage.h"
#include <assert.h>
#include <functional>
#include <limits>
#include <memory>

namespace Dynamic {
	template<class T>
	class Array {
	public:
		/**
		 * Constructor
		 * @param size the size of array to allocate at the start
		 */
		explicit Array(const size_t size = 0);

		/** Copy Constructor, shallow copies the entire array */
		Array(const Array<T>& rhs);

		/** Get the dumb array pointer */
		inline const Storage& getRaw() { return array; } const;

		/** Override the array bracket operator */
		const T& operator[](const size_t index) const { return array[index]; };
		T& operator[](const size_t index) { return array[index]; };

		/**
		 * allow pushing back elements to the array
		 * @param T& elem teh element to add to the array
		 * NOTE: By adding the element, please ensure the element is only accesible from the Array, as the Array may
		 * decide to destroy the element before you're done with it otherwise.
		 */
		size_t append(const T& elem);

		/**
		 * Extend this Array with another array's data
		 * @param ray The dynamic array to pull data from
		 * @param length The length of the array, if it's a static array
		 */
		size_t extend(const Array<T>& extendRay);
		size_t extend(const T extendRay[]);

		/**
		 * Insert an element (or an array) at a given position (warning: slow)
		 * @param index where to insert the data
		 * @param elem the data(m) to insert
		 */
		size_t insert(const size_t index, const T& elem);
		size_t insert(const size_t index, const T insertRay[]);
		size_t insert(const size_t index, const Array<T>& insertRay);

		/** removes an element in the list */
		size_t remove(const size_t index);

		/** removes an element from the list, and return the element */
		T pop();
		T pop(const size_t index);

		/** emptey the array, deleting all elements */
		void clear();

		/**
		 * Get the index of the first element in the list that is equal to x
		 * @param elem the element to check equality for
		 * @param start the index of the list to start the search
		 * @param end the index of the list to end the search
		 * @returns the index of the element being searched for, -1 if none
		 */
		int index(const T& elem, const size_t start = 0) const;
		int index(const T& elem, const size_t start, const size_t end) const;

		/**
		 * Count the number of elements in a list that are equal to x
		 * @param elem the element to check equality for
		 * @param start the index of the list to start the search
		 * @param end the index of the list to end the search
		 * @returns the numer of elements that match x in the given range
		 */
		size_t count(const T& elem, const size_t start = 0) const;
		size_t count(const T& elem, const size_t start, const size_t end) const;

		/**
		 * Sorts the list in-place, based on the JavaScript sort prototype
		 * {@link https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/sort}
		 * Uses TimSort because python
		 * @param reverse whether or not to sort the list reversed
		 * @param sortFunc function that takes two elements of the list
		 */
		void sort(bool reverse = false);
		void sort(bool reverse, std::function<int(const T&, const T&)> sortFunc);

		/**
		 * Resize the array based on a start and end offset,
		 * similair to slice() in other languages except it works in place
		 * @param start the index to start the new array, inclusive
		 * @param end the index to end the new array, exclusive
		 */
		size_t resize(const size_t end);
		size_t resize(const size_t start, const size_t end);

		inline size_t size() const { return array.size() };
		inline size_t allocated() const { return array.allocated(); }
	private:
		/**
		 *	Utility function to replace array with an expanded array
		 *	@param newCapacity the new capacity of array, in units of T
		 */
		void m_expand(const size_t newCapacity);
		/**
		 * Another Utility function, checks if the capacity of the array
		 * is large enough to support n additional elements, and if not
		 * expands it
		 * @param newElemCount the number of new elements we want to add
		 * @returns the new capacity
		 */
		size_t m_checkExpansion(const size_t newElemCount);
		/** the storage */
		Storage m_array;
	};
};