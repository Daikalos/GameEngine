#pragma once

#include <functional>
#include <array>

#include <Velox/Utility/ContainerUtils.h>

namespace vlx
{
	template<class I>
	concept HasButtonInput = requires(I input)
	{
		{ input.Pressed(typename I::ButtonType()) } -> std::same_as<bool>;
		{ input.Released(typename I::ButtonType()) } -> std::same_as<bool>;
		{ input.Held(typename I::ButtonType()) } -> std::same_as<bool>;
	};

	enum ButtonTrigger : uint8
	{
		BT_Pressed,
		BT_Released,
		BT_Held,
	};

	///	ButtonEvent is small container class for mapping functions to individual key events. This was created to 
	/// have the same lifetime as the provided functions have in order to prevent easy-to-miss errors.
	/// 
	template<class T> requires HasButtonInput<T>
	class ButtonEvent final
	{
	private:
		using ButtonType = typename T::ButtonType;

	private:
		struct ButtonCallback
		{
			std::function<void()>	func;
			ButtonType				button;				// button type to listen for
			ButtonTrigger			trigger;			// button trigger to listen for
			float					priority {0.0f};	// determines position in callbacks
		};

	public:
		ButtonEvent() = default;
		ButtonEvent(const T& input);
	
	public:
		template<typename Func>
		void Add(ButtonType button, ButtonTrigger trigger, Func&& func, float priority = 0.0f)
		{
			cu::InsertSorted<ButtonCallback>(m_callbacks, ButtonCallback(std::forward<Func>(func), button, trigger, priority),
				[](const ButtonCallback& lhs, const ButtonCallback& rhs)
				{
					return lhs.priority > rhs.priority;
				});
		}

		template<typename Func, class U>
		void Add(ButtonType button, ButtonTrigger trigger, Func&& func, U* object, float priority = 0.0f)
		{
			Add(button, trigger, std::bind(std::forward<Func>(func), object), priority); // to allow adding member functions with the hidden this pointer as parameter
		}

	public:
		void Execute();
		void operator()();

		void Execute(const T& input);
		void operator()(const T& input);

		void Clear();

	private:
		std::vector<ButtonCallback> m_callbacks;
		const T* m_input {nullptr};
	};

	template<class T> requires HasButtonInput<T>
	inline ButtonEvent<T>::ButtonEvent(const T& input) 
		: m_input(&input) { }

	template<class T> requires HasButtonInput<T>
	inline void ButtonEvent<T>::Execute()
	{
		if (m_input != nullptr)
			Execute(*m_input);
	}
	template<class T> requires HasButtonInput<T>
	inline void ButtonEvent<T>::operator()()
	{
		Execute();
	}
	
	template<class T> requires HasButtonInput<T>
	inline void ButtonEvent<T>::Execute(const T& input)
	{
		const auto CheckTriggered = [&input](const ButtonCallback& callback) -> bool
		{
			switch (callback.trigger)
			{
			case BT_Pressed:	return input.Pressed(callback.button);
			case BT_Released:	return input.Released(callback.button);
			case BT_Held:		return input.Held(callback.button);
			default:			return false;
			}
		};

		for (const auto& callback : m_callbacks)
		{
			if (CheckTriggered(callback) && callback.func)
				callback.func();
		}
	}
	template<class T> requires HasButtonInput<T>
	inline void ButtonEvent<T>::operator()(const T& input)
	{
		Execute(input);
	}

	template<class T> requires HasButtonInput<T>
	inline void ButtonEvent<T>::Clear()
	{
		m_callbacks.clear();
	}


}