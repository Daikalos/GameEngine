#include "Component.h"

using namespace fge;

template<class T>
void Component<T>::construct_data(unsigned char* data) const
{
	new (&data[0]) T();
}

template<class T>
void Component<T>::destroy_data(unsigned char* data) const
{
	T* data_location = std::launder(reinterpret_cast<T*>(data)); // launder allows for changing the type of object (makes the type cast legal)
	data_location->~T();
}

template<class T>
void Component<T>::move_data(unsigned char* source, unsigned char* destination) const
{
	new (&destination[0]) T(std::move(*reinterpret_cast<T*>(source)));
}

template<class T>
std::size_t Component<T>::get_size() const
{
	return sizeof(T);
}

template<class T>
ComponentTypeID Component<T>::get_type_id()
{
	return TypeIdGenerator<ComponentBase>::get_new_id<T>();
}