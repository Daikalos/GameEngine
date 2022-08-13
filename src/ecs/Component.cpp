#include "Component.h"

using namespace fge;

template<class C>
void Component<C>::construct_data(unsigned char* data) const
{
	new (&data[0]) C(); // construct object C at data in memory which will give its values
}

template<class C>
void Component<C>::destroy_data(unsigned char* data) const
{
	C* data_location = std::launder(reinterpret_cast<C*>(data)); // launder allows for changing the type of object (makes the type cast legal)
	data_location->~C();
}

template<class C>
void Component<C>::move_data(unsigned char* source, unsigned char* destination) const
{
	new (&destination[0]) C(std::move(*reinterpret_cast<C*>(source))); // move the data in src by constructing a object at dest with the values from src
}

template<class C>
std::size_t Component<C>::get_size() const
{
	return sizeof(C);
}

template<class C>
ComponentTypeID Component<C>::get_type_id()
{
	return TypeIdGenerator<ComponentBase>::get_new_id<C>();
}