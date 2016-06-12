#ifndef TEMPLATE_VECTOR_HPP
#define TEMPLATE_VECTOR_HPP

#include <iostream>
#include <type_traits>
#include <assert.h>
#include <stdexcept>
#include <initializer_list>

template<class T>
class Vector
{
private:
	size_t length;
	size_t cap;
	const static size_t default_capacity = 32;
	T *v;

	// Methods
	void initialize(const std::size_t, const T t = T{}, const std::size_t = default_capacity);
	void copy(const Vector &);
	void double_capacity();
	void reallocate(const std::size_t);

public:
	// Default constructor
	Vector<T>();

	// Constructor that initializes as size_t l default objects
	explicit Vector(const std::size_t);
	explicit Vector(const std::size_t, T);

	// Constructor from initializer list of T's
	explicit Vector(std::initializer_list<T>);

	~Vector();
	explicit Vector(const Vector<T> &);
	explicit Vector(Vector<T>&&);

	// Operators
	Vector& operator=(const Vector &);
	Vector& operator=(Vector &&);
	T& operator[](const size_t);
	const T& operator[](const size_t) const;

	// Methods
	void reset(const T t = T{});
	inline std::size_t size() const { return length; }
	inline std::size_t capacity() const { return cap; }
	void push_back(T);
	void insert(const std::size_t, T);
	void clear();
	void erase(const std::size_t);

	// Random access iterator: http://en.cppreference.com/w/cpp/concept/RandomAccessIterator

	T * begin();						// "A pointer to an element of an array satisfies all requirements of RandomAccessIterator" http://en.cppreference.com/w/cpp/concept/RandomAccessIterator
	T const * begin() const;			// A pointer to a const T, in case we want to iterate through a const array
	T * end();
	T const * end() const;
	T * find(T const &);
	T const * find(T const &) const;

	// For debugging
	void printVector() const;
	//inline T * & get_vector() { return v; }


};
template<typename T>
Vector<T>::Vector()
{
	initialize(0);
}

// Initialize a vector with l default initialized T's
template<typename T>
Vector<T>::Vector(const size_t l) {
	initialize(l);
}
// Initialize a vector of size l, containing t.
template<typename T>
Vector<T>::Vector(const size_t l, T t) {
	initialize(l, t);
}
// Initializer-list constructor
template<typename T>
Vector<T>::Vector(std::initializer_list<T> iList)
{
	static_assert(std::is_move_constructible<T>::value, "Must be move constructable");
	static_assert(std::is_move_assignable<T>::value, "Must be move assignable");
	cap = default_capacity;
	length = iList.size();
	while (cap < length) {
		cap = cap * 2;
	}
	v = new T[cap];

	// For each place in v, copy the element from iList att corresponding place
	for (size_t i = 0; i < length; i++)
	{
		v[i] = *(iList.begin() + i);
	}
}
template<typename T>
Vector<T>::Vector(const Vector &other)
{
	copy(other);
}
template<typename T>
Vector<T>::Vector(Vector &&other)
{
	cap = other.cap;
	length = other.length;
	v = other.v;

	// Clear moved from
	other.length = 0;
	other.cap = 0;
	other.v = nullptr;
}
// Desctructor
template<typename T>
Vector<T>::~Vector()
{
	delete[] v;
}
template<typename T>
Vector<T>& Vector<T>::operator=(const Vector &other)
{
	if (this != &other)
	{
		delete[] v;			// Free the old array
		copy(other);
	}
	return *this;
}
template<typename T>
Vector<T>& Vector<T>::operator=(Vector &&other)
{
	if (this != &other)
	{
		cap = other.cap;
		length = other.length;
		delete[] v;
		v = other.v;

		// Clear moved from
		other.length = 0;
		other.cap = 0;
		other.v = nullptr;
	}
	return *this;
}
template<typename T>
T& Vector<T>::operator[](const size_t i)
{
	if (i < length)
		return v[i];
	else
		throw std::out_of_range("Index out of bounds.");
}
template<typename T>
const T& Vector<T>::operator[](const size_t i) const
{
	if (i < length)
		return v[i];
	else
		throw std::out_of_range("Index out of bounds.");
}
// Methods
//  intitialize(..) sets length equal to l and capacity equal to default capacity
template<typename T>
void Vector<T>::initialize(const std::size_t l, const T t, const std::size_t c)
{
	static_assert(std::is_move_constructible<T>::value, "Must be move constructable");
	static_assert(std::is_move_assignable<T>::value, "Must be move assignable");
	length = l;
	cap = c;
	// Double capacity until it is larger or equal to length
	while (cap < length) {
		cap = cap * 2;
	}
	v = new T[cap];
	reset(t);
}
template<typename T>
void Vector<T>::reset(const T t)
{
	for (std::size_t i = 0; i < length; i++)
	{
		v[i] = t;
	}
}

// Copy the elements from the other vector into this one
template<typename T>
void Vector<T>::copy(const Vector &other)
{
	if (this != &other) {
		cap = other.cap;
		length = other.length;
		v = new T[length];
		for (std::size_t i = 0; i < length; ++i)
			v[i] = other.v[i];
	}
}
template<typename T>
void Vector<T>::double_capacity() {
	reallocate(cap * 2);
}
template<typename T>
void Vector<T>::reallocate(const std::size_t new_cap) {
	// Create an array with the new size and copy over the elements
	T * new_array = new T[new_cap];
	for (std::size_t i = 0; i < length; ++i) {
		new_array[i] = v[i];
	}
	// delete the old array, and point v to the new one.
	delete[] v;
	cap = new_cap;
	v = new_array;
}
template<typename T>
void Vector<T>::push_back(T t) {
	if (length + 1 > cap)
		double_capacity();
	v[length] = t;
	++length;
}
template<typename T>
void Vector<T>::insert(const std::size_t pos, T t) {
	// Check if within bounds
	if (pos > length || pos < 0)
		throw std::out_of_range("Trying to insert at a position outside range");

	// If insert will lead to exceeding of capacity => double capacity
	if (length + 1 > cap)
		double_capacity();

	// Flytta alla element efter insert-platsen ett steg, och sätt sedan in
	for (std::size_t i = length; i > pos; --i) {
		v[i] = v[i - 1];
	}
	v[pos] = t;
	++length;
}
// Clears the vector but keeps the capacity.
template<typename T>
void Vector<T>::clear() {
	length = 0;
}
template<typename T>
void Vector<T>::erase(const std::size_t pos) {
	if (pos >= length || pos < 0)
		throw std::out_of_range("Trying to erase at a position outside range");
	for (std::size_t i = pos; i < length - 1; ++i) {
		v[i] = v[i + 1];
	}
	--length;
}
template<typename T>
T * Vector<T>::begin() {
	return &v[0];
}
template<typename T>
T const * Vector<T>::begin() const {
	return &v[0];
}
template<typename T>
T * Vector<T>::end() {
	return &v[0 + length];
}
template<typename T>
T const * Vector<T>::end() const {
	return &v[0 + length];
}
template<typename T>
T * Vector<T>::find(const T &elem) {
	for (T * beg = &v[0]; beg != &v[length]; ++beg) {
		if (*beg == elem)
			return beg;
	}
	return &v[length];
}
template<typename T>
T const * Vector<T>::find(const T &elem) const {
	for (T * beg = &v[0]; beg != &v[length]; ++beg) {
		if (*beg == elem)
			return beg;
	}
	return &v[length];
}
template<typename T>
void Vector<T>::printVector() const {
	std::cout << "Vector size: " << length << std::endl;
	std::cout << "Vector capacity: " << cap << std::endl;
	std::cout << "{ ";
	for (std::size_t i = 0; i < length; ++i) {
		std::cout << v[i] << " ";
	}
	std::cout << "}" << std::endl;
}
#endif

