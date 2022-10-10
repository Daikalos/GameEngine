#pragma once

#include <type_traits>

namespace vlx::traits
{
    template <typename T>
    struct FunctionTraits : public FunctionTraits<decltype(&std::remove_reference_t<T>::operator())> {};

    template <typename ReturnType, typename... Args>
    struct FunctionTraits<ReturnType(*)(Args...)> : FunctionTraits<ReturnType(Args...)> {};

    template <typename ReturnType, typename ClassType, typename... Args>
    struct FunctionTraits<ReturnType(ClassType::*)(Args...) const> : FunctionTraits<ReturnType(ClassType::*)(Args...)> {};

    template <typename ReturnType, typename ClassType, typename... Args>
    struct FunctionTraits<ReturnType(ClassType::*)(Args...)> : FunctionTraits<ReturnType(Args...)>
    {
        using class_type = ClassType;
    };

    template <typename ReturnType, typename... Args>
    struct FunctionTraits<ReturnType(Args...)>
    {
        using arguments = std::tuple<Args...>;

        static constexpr std::size_t arity = std::tuple_size_v<arguments>;
        
        using result_type = ReturnType;

        template <std::size_t Index>
        using arg_type = std::tuple_element_t<Index, std::tuple<Args...>>;
    };

    template<typename T>
    struct Base {};

    template<typename... Ts> struct NoDuplicates : Base<Ts>...
    {
        constexpr operator bool() const { return true; }
    };
}
