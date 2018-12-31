#pragma once
/**
 * Exception saftey is difficult. One solution I am attempting is to have fixed capacity
 * storage class (Dynamic::Storage) which is then utilized by Dynamic::Array to create
 * a dynamic capacity array. This should hopefully make my memory less prone to leaking
 * during catastrophic failure when I am doing things such as copy/swap.
 */

#include <assert.h>
#include <algorithm> 

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

		/** Dtor */
		~Storage();

		/** get the raw pointer to play with it */
		inline const T* getRaw() const { return arrayPtr };

		/** Override the array bracket operator */
		const T& operator[](const size_t index) const;
		T& operator[](const size_t index);

		/**
		 * Assign using a deep copy.
		 */
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
		size_t append(const T elems[], const size_t count, const size_t start = 0);

		/**
		 * allow deleting the last element of the array
		 * @returns the new length of the array
		 *
		 */
		size_t truncate(size_t count = 1);

		/** getters */
		inline size_t size() const { return length;  }
		inline size_t allocated() const { return capacity; }
	private:
		/**
		 * Utility function to allocate the weird array needed
		 * @param size the size of the array to allocate, in units of T
		 */
		static T* alloc(const size_t size);

		/**
		 * Same as above, but deallocating instead
		 * @param ray the array to deallocate
		 * @param size the number of constructed elements in the array
		 * @param allocated the size of the array, in units of T
		 */
		static void dealloc(T* ray, const size_t size);

		const size_t capacity;
		size_t length;
		T* arrayPtr;
		/** used when swapping the arrayPtr to make sure that all memory is deallocated */
		T* tempArrayPtr = nullptr;
		size_t tempLength = 0;
	};
};