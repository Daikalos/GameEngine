#pragma once

#include <type_traits>

#include "IDGenerator.h"

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

    template<typename... Ts> 
    struct NoDuplicates : Base<Ts>... // can't inherit from the same base multiple times, therefore error
    {
        constexpr operator bool() const { return true; }
    };

    namespace hidden
    {
        template<std::size_t Index, typename T, typename Tuple>
        static constexpr std::size_t IndexInTupleFn()
        {
            static_assert(Index < std::tuple_size_v<Tuple>, "The element is not in the tuple");
            if constexpr (std::is_same_v<T, std::tuple_element_t<Index, Tuple>>)
            {
                return Index;
            }
            else
            {
                return IndexInTupleFn<Index + 1, T, Tuple>();
            }
        }
    }

    /// Returns in compile-time the index to where in tuple the type is located at
    /// 
    template<typename T, typename Tuple>
    struct IndexInTuple
    {
        static constexpr size_t value = hidden::IndexInTupleFn<0, T, Tuple>();
    };

    /// Sorts a tuple given a comparator, e.g., DescendingID
    ///
    namespace sort
    {
        template<bool B>
        struct Conditional 
        {
            template<class T, class> using type = T;
        };

        template<>
        struct Conditional<false> 
        {
            template<class, class U> using type = U;
        };

        template<class T, class U>
        using DescendingID = Conditional<(id::Type<T>::ID() > id::Type<U>::ID())>;

        template<class T, class U>
        using AscendingID = Conditional<(id::Type<T>::ID() < id::Type<U>::ID())>;

        template <std::size_t I, std::size_t J, class Tuple>
        class TupleElementSwap
        {
            template <class>
            struct TupleElementSwapImpl;

            template <std::size_t... Is>
            struct TupleElementSwapImpl<std::index_sequence<Is...>>
            {
                using type = std::tuple<std::tuple_element_t<Is != I && Is != J ? Is : Is == I ? J : I, Tuple>...>;
            };

        public:
            using type = typename TupleElementSwapImpl<std::make_index_sequence<std::tuple_size_v<Tuple>>>::type;
        };

        template <template <class, class> class Comparator, class Tuple>
        class TupleSelectionSort
        {
            template<size_t I, size_t J, size_t N, class LoopTuple>
            struct TupleSelectionSortImpl
            {
                using NextLoopTuple = typename Comparator<
                    std::tuple_element_t<I, LoopTuple>, 
                    std::tuple_element_t<J, LoopTuple>>::template type<typename TupleElementSwap<I, J, LoopTuple>::type, LoopTuple>;

                using type = typename TupleSelectionSortImpl<I, J + 1, N, NextLoopTuple>::type;
            };

            template <std::size_t I, std::size_t TupleSize, class LoopTuple>
            struct TupleSelectionSortImpl<I, TupleSize, TupleSize, LoopTuple>
            {
                // once j == tuple_size, we increment i and start j at i + 1 and recurse
                using type = typename TupleSelectionSortImpl<I + 1, I + 2, TupleSize, LoopTuple>::type;
            };

            template <std::size_t j, std::size_t TupleSize, class LoopTuple>
            struct TupleSelectionSortImpl<TupleSize, j, TupleSize, LoopTuple>
            {
                // once i == tuple_size, we know that every element has been compared
                using type = LoopTuple;
            };

        public:
            using type = typename TupleSelectionSortImpl<0, 1, std::tuple_size_v<Tuple>, Tuple>::type;
        };
    }
}
