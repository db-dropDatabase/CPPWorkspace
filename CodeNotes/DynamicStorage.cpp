#include "DynamicStorage.h"

using namespace Dynamic;

template<class T>
Storage<T>::Storage(const size_t size = 0)
	: m_capacity(size)
	, m_length(0)
	, m_arrayPtr(s_alloc(m_capacity)) {
	// sanity check
	assert(m_capacity < 1000);
}

template<class T>
Storage<T>::Storage(const Storage<T>& copyFrom, const size_t size)
	: m_capacity(size)
	, m_length(0)
	, m_arrayPtr(s_alloc(m_capacity)) {
	// sanity!
	assert(m_capacity >= copyFrom.allocated());
	assert(m_capacity < 1000);
	assert(copyFrom.size() <= m_capacity);
	// copy the elements from copyFrom to our array
	append(copyFrom);
}

template<class T>
Storage<T>::Storage(const Storage<T>& rhs)
	: m_capacity(rhs.allocated())
	, m_length(0)
	, m_arrayPtr(s_alloc(m_capacity)) {
	// sanity!
	assert(m_capacity < 1000);
	assert(rhs.size() <= m_capacity);
	// copy the elements from copyFrom to our array
	append(rhs);
}

template<class T>
Storage<T>::~Storage() {
	// deallocate our array!
	m_deallocSelf();
}

template<class T>
Storage<T>& Storage<T>::operator=(Storage<T>&& rhs) {
	// ensure we're not moving rhs into istelf
	assert(this != &rhs);
	// copy and swap, though nice, is rather slow
	// instead we switch the storage objects, and deallocate one of them
	std::swap(rhs.m_arrayPtr, m_arrayPtr);
	std::swap(rhs.m_length, m_length);
	std::swap(rhs.m_capacity, m_capacity);
	// deallocate rhs
	rhs.m_deallocSelf();
	return *this;
}

template<class T>
Storage<T>& Storage<T>::operator=(const Storage<T>& rhs) {
	// copy and swap!
	*this = std::move(Storage(rhs));
	return *this;
}

template<class T>
const T& Storage<T>::operator[](const size_t index) const {
	assert(index < m_length);
	return m_arrayPtr[index];
}

template<class T>
T& Storage<T>::operator[](const size_t index) {
	assert(index < m_length);
	return m_arrayPtr[index];
}

template<class T>
size_t Storage<T>::append(const T elems[], const size_t count, const size_t start = 0) {
	// if the # of new elements and the length exceed our capacity
	if (m_length + count > m_capacity) {
		// use the move operator on ourselves, with a temporary copy of ourselves that is expanded
		*this = std::move(Storage(*this, s_tH(m_length + count)));
	}
	// start copying using placement new, incrementing the length as we go!
	for (size_t i = 0; i < count; m_length++, i++) ::new (&m_arrayPtr[m_length]) T(elems[i]);
	// return the new length
	return m_length;
}

template<class T>
size_t Storage<T>::truncate(size_t count = 1) {
	assert(count <= m_length);
	// decrement length, and call dtor of the removed object
	for (; count > 0; --m_length, count--) m_arrayPtr[m_length].~T();
	// return the new length
	return m_length;
}

template<class T>
inline T* Storage<T>::s_alloc(const size_t size) {
	return std::allocator<T>().allocate(size);
}

template<class T>
template<typename I>
inline I Storage<T>::s_tH(const I num) {
	// catch integer overflow for type
	assert(num < std::numeric_limits<I>::max() / 3 - 1);
	return ((num * 3) >> 1) + 1;
}

template<class T>
inline void Storage<T>::m_deallocSelf() {
	if (m_arrayPtr != nullptr) {
		// call the dtors of all the elements
		for (; m_length > 0; m_length--) m_arrayPtr[m_length - 1].~T();
		// deallocate the array
		std::allocator<T>().deallocate(m_arrayPtr, m_capacity);
		// set it to nullptr for saftey
		m_arrayPtr = nullptr;
		// and reset the capacity
		m_capacity = 0;
	}
}