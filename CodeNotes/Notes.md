# Notes from my C++ books

C++, similar to JavaScript, seems to be filled with hidden implicit actions, so I'm going to go ahead and keep a list of things I might trip on here.

### Don't Put Spaces In Filenames
It breaks GDB.

### `explicit` In Constructor
Apparently there are some cases where C++ will do implicit type conversions using a built-in constructor. Example:
```C++
B bObj1(24);
doSomething(bObj1); // OK! the function accepts type B
// But what if we put a number in it?
doSomething(24); // C++ might try and implicitly convert 24 into B(24), which is bad
// to prevent this, declare all contructors like
explicit B(int x);
// that way the compilier throws an error instead of our program catching on fires
```

### Constructor Behavior

The default constructor is invoked if the object is declared without arguments. The copy constructor is invoked if an object is being created (such as pass-by-value). The copy assignment operator is invoked if the object is being copied. For example:

```C++
class Thing {
public:
    Thing(); // default constructor
    Thing(const Thing& rhs); // copy constructor
    Thing& operator=(const Thing& rhs); // copy assignment operator
};

// Default contructor
Thing th1;
// Copy constructor
Thing th2(th1);
// Also copy constructor
Thing th3 = th2
// if a function is pass by value, this also calls the copy constructor
doThing(th1);
// copy assignment operator
th1 = th2;
```

### Misc

Static in C++ works the same as in Java!

Constant pointers:
```C++
// non-const
char * author = "Noah Koontz";
// const pointer, non-const data
const char * author = "Noah Koontz";
// non-const pointer, const data
char * const author = "Noah Koontz";
// const both
const char * const author = "Noah Koontz";
```

Constant functions:
```C++
// not constant
int dothing(int thing);
// constant return value (cannot be changed)
const int dothing(int thing);
// constant parameter (cannot be changed in function)
int dothing(const int thing);
// constant member function (cannot change anything in "this")
int someclass::dothing(int thing) const;
```

Max template function (instead of preprocessor macro):
```C++
template<typename T> inline void max(const T& a, const T& b) {
    return a > b ? a : b;
}
```