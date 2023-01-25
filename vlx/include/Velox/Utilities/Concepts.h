#pragma once

#include <concepts>
#include <filesystem>

#include "Traits.h"

namespace vlx // concepts is the best thing ever
{
	template<typename T>
	concept Arithmetic = std::is_arithmetic_v<T>;

	template<typename T>
	concept Integral = std::is_integral_v<T>;

	template<typename T>
	concept Enum = std::is_enum_v<T>;

	template<typename T>
	concept ArithEnum = Arithmetic<T> || Enum<T>;

	template<typename T>
	concept IntEnum = Integral<T> || Enum<T>;

	template<typename T, typename... Args>
	concept IsSameType = std::conjunction_v<std::is_same<T, Args>...>;

	template<typename T>
	concept IsStringType = std::is_convertible_v<T, std::string_view>;

	template<typename... Args>
	concept Exists = sizeof...(Args) > 0;

	template<std::size_t sz, typename... Args>
	concept IsSameSize = requires 
	{
		sz == (sizeof(Args) + ... + 0); 
	};

	template<typename T, typename... Args>
	concept IsPresent = (std::same_as<T, Args> || ...);

	template<class T>
	concept IsVector = requires
	{
		std::same_as<T, std::vector<typename T::value_type>>;
	};

	template <class ContainerType>
	concept IsContainer = requires(ContainerType a, const ContainerType b)
	{
		requires std::regular<ContainerType>;
		requires std::swappable<ContainerType>;
		requires std::destructible<typename ContainerType::value_type>;
		requires std::same_as<typename ContainerType::reference, typename ContainerType::value_type&>;
		requires std::same_as<typename ContainerType::const_reference, const typename ContainerType::value_type&>;
		requires std::forward_iterator<typename ContainerType::iterator>;
		requires std::forward_iterator<typename ContainerType::const_iterator>;
		requires std::signed_integral<typename ContainerType::difference_type>;
		requires std::same_as<typename ContainerType::difference_type, typename std::iterator_traits<typename ContainerType::iterator>::difference_type>;
		requires std::same_as<typename ContainerType::difference_type, typename std::iterator_traits<typename ContainerType::const_iterator>::difference_type>;
		{ a.begin() } -> std::same_as<typename ContainerType::iterator>;
		{ a.end() } -> std::same_as<typename ContainerType::iterator>;
		{ b.begin() } -> std::same_as<typename ContainerType::const_iterator>;
		{ b.end() } -> std::same_as<typename ContainerType::const_iterator>;
		{ a.cbegin() } -> std::same_as<typename ContainerType::const_iterator>;
		{ a.cend() } -> std::same_as<typename ContainerType::const_iterator>;
		{ a.size() } -> std::same_as<typename ContainerType::size_type>;
		{ a.max_size() } -> std::same_as<typename ContainerType::size_type>;
		{ a.empty() } -> std::same_as<bool>;
	};

	template<typename... Ts>
	concept NoDuplicates = requires
	{
		traits::NoDuplicates<Ts...>{};
	};

	template<class Lambda, typename T, std::size_t... Index>
	concept SameTypeParameter = (std::same_as<T, std::decay_t<typename traits::FunctionTraits<Lambda>::template arg_type<Index>>> && ...);

	template<class T, class... Args>
	concept Contains = std::disjunction_v<std::is_same<T, Args>...>;

	// UNIQUE

	class IComponent;

	template<class Resource>
	concept IsLoadable = requires(Resource resource)
	{
		{ resource.loadFromFile(std::filesystem::path()) } -> std::same_as<bool>;
	};

	template<class Input, typename Bind>
	concept IsButtonInput = requires(Input input)
	{
		{ input.Pressed(Bind()) } -> std::same_as<bool>;
		{ input.Released(Bind()) } -> std::same_as<bool>;
		{ input.Held(Bind()) } -> std::same_as<bool>;
	};

	template<class C>
	concept IsComponent = std::derived_from<C, IComponent> && std::is_class_v<C> && std::semiregular<C> && sizeof(C) > 1;

	template<class... Cs>
	concept IsComponents = (IsComponent<Cs> && ...) && Exists<Cs...> && NoDuplicates<Cs...>;
}