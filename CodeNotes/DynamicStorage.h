#pragma once
/**
 * Exception saftey is difficult. One solution I am attempting is to have fixed capacity
 * storage class (Dynamic::Storage) which is then utilized by Dynamic::Array to create
 * a dynamic capacity array. This should hopefully make my memory less prone to leaking
 * during catastrophic failure when I am doing things such as copy/swap.
 */

#include <assert.h>
#include <algorithm> 
#include <utility>

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
		Storage(Storage<T>&& rhs) 
			: capacity(0)
			, length(0)
			, arrayPtr(nullptr) { *this = std::move(rhs); }

		/** Dtor */
		~Storage();

		/** get the raw pointer */
		inline const T* getRaw() const { return arrayPtr };

		/** Override the array bracket operator */
		const T& operator[](const size_t index) const;
		T& operator[](const size_t index);

		/** Assign by transfering ownership of the data block */
		Storage<T>& operator=(Storage<T>&& rhs);
		/** Assign by creating a copy */
		Storage<T>& operator=(const Storage<T> rhs);

		/**
		 * allow pushing back elements to the array
		 * @param elems the element(s) to add to the array
		 * @param count the number of elements to append from elems
		 * @param start the position in elems to start appending from
		 * NOTE: By adding the element, please ensure the element is only accesible from the Array, as the Array may
		 * decide to destroy the element before you're done with it otherwise.
		 * @returns the new length of the array
		 */
		inline size_t append(const T& elem) { const T ray[] = { elem }; return append(ray, 1); }
		inline size_t append(const Storage& elems) { return append(elems, elems.size()); }
		inline size_t append(const Storage& elems, const size_t count, const size_t start = 0) { return append(elems.getRaw(), count, start); }
		size_t append(const T elems[], const size_t count, const size_t start = 0);

		/**
		 * allow deleting the last element of the array
		 * @returns the new length of the array
		 */
		size_t truncate(size_t count = 1);

		/** getters */
		inline size_t size() const { return length;  }
		inline size_t allocated() const { return capacity; }
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
		
		/**
		 * Utility function to check if the array needs expanding, and if so expand it
		 * @param newElemCount the # of new elements we would like to add
		 * @returns the new capacity
		 */
		size_t m_checkExpansion(const size_t newElemCount);

		size_t m_capacity;
		size_t m_length;
		T* m_arrayPtr;
	};
};