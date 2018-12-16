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
 */

template<class T>
class DynamicArray {
    public:
        /**
         * Constructor
         * @param length the size of array to allocate at the start
         */
        explicit DynamicArray(const size_t length = 0);
        
        /** Copy Constructor, deep copies the entire array */
        DynamicArray(DynamicArray<T>& rhs);

        /** Get the dumb array pointer */
        const T* getRaw() const;

        /** Override the array bracket operator */
        const T& operator[](const size_t index) const;
        T& operator[](const size_t index);

        /** 
         * allow pushing back elements to the array 
         * @param T& elem teh element to add to the array
         * NOTE: By adding the element, please ensure the element is only accesible from the DynamicArray, as the DynamicArray may
         * decide to destroy the element before you're done with it otherwise.
         */
        size_t push(const T& elem);

        /** reduce the size of the vector by one */
        size_t pop();

        /** 
         * Extend this DynamicArray with another array's data
         * @param ray The dynamic array to pull data from
         * @param length The length of the array, if it's a static array
         */
        size_t extend(const DynamicArray<T>& ray);
        size_t extend(const T[] ray, const size_t length);

        size_t length() const;
        size_t capacity() const;
    private:
        T* array;
        unsigned int length;
        unsigned int capacity;
}

