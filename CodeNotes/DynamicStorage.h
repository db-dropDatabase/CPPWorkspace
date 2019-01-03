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

#include <assert.h>
#include <algorithm> 
#include <utility>
#include <memory>

namespace Dynamic {
	template<class T>
	class Storage {
	public:
		/**
		 * Ctor
		 * @param ray an array to copy elements from
		 * @param size the size of array to allocate, in units of T
		 */
		explicit Storage(const size_t size = 0);
		explicit Storage(const Storage<T>& copyfrom, const size_t size);

		/** Copy constructor, performs a deep copy using placement new of the same capacity */
		Storage(const Storage<T>& rhs);

		/** Move ctor, transfer ownership of the memory block (with code in assignment operator) */
		Storage(Storage<T>&& rhs) noexcept;

		/** Dtor */
		~Storage();

		/** get the raw pointer */
		const T* getRaw() const { return m_arrayPtr; };

		/** Override the array bracket operator */
		const T& operator[](const size_t index) const;
		T& operator[](const size_t index);

		/** Assign by transfering ownership of the data block */
		Storage<T>& operator=(Storage<T>&& rhs) noexcept;
		/** Assign by creating a copy */
		Storage<T>& operator=(const Storage<T>& rhs);

		/**
		 * allow pushing back elements to the array
		 * @param elems the element(s) to add to the array
		 * @param count the number of elements to append from elems
		 * @param start the position in elems to start appending from
		 * NOTE: By adding the element, please ensure the element is only accesible from the Array, as the Array may
		 * decide to destroy the element before you're done with it otherwise.
		 * @returns the new length of the array
		 */
		size_t append(const T& elem) { const T ray[] = { elem }; return append(ray, 1); }
		size_t append(const Storage& elems) { return append(elems, elems.size()); }
		size_t append(const Storage& elems, const size_t count, const size_t start = 0) { return append(elems.getRaw(), count, start); }
		size_t append(const T elems[], const size_t count, const size_t start = 0);

		/**
		 * allow pushing to the back, but by constructing the object in place instead of the standard copy-into-place
		 * @param elem the element to add to the array
		 * @returns the new length of the array
		 */
		template<class... Args>
		size_t emplace(Args&& ... args);

		/**
		 * allow deleting the last element of the array
		 * @returns the new length of the array
		 */
		size_t truncate(size_t count = 1);

		/** getters */
		size_t size() const { return m_length;  }
		size_t allocated() const { return m_capacity; }
	private:
		/**
		 * utility function to do a cheap 3/2, always rounding up.
		 * NOTE: will always return a number > or < 0
		 * @param number the number to multiply by 1.5
		 */
		template<typename I>
		static I s_tH(const I num);

		/**
		 * Utility function to allocate the weird array needed
		 * @param size the size of the array to allocate, in units of T
		 */
		static T* s_alloc(const size_t size);
		
		/**
		 * Same as above, but deallocating instead. Sets arrayPtr to nullptr, and zeros internal vars
		 */
		void m_deallocSelf();

		size_t m_capacity;
		size_t m_length;
		T* m_arrayPtr;
	};
};