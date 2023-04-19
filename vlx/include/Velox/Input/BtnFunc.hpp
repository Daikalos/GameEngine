#pragma once

#include <functional>
#include <array>

#include "InputHandler.h"

namespace vlx
{
	enum ButtonEvent : uint8
	{
		BE_Begin	= 0,

		BE_Pressed	= 0,
		BE_Released = 1,
		BE_Held		= 2,

		BE_End		= 3
	};

	inline ButtonEvent& operator++(ButtonEvent& button_event)
	{
		button_event = static_cast<ButtonEvent>(static_cast<uint8>(button_event) + 1);
		return button_event;
	}

	///	BtnFunc is small container class for mapping functions to individual key events. This was created to 
	/// have the same lifetime as the provided functions have in order to prevent common errors. It also allows 
	/// for the developer to determine when the functions should be executed.
	/// 
	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	class BtnFunc final
	{
	private:
		struct BoundFunc
		{
			std::function<void()>	func;
			ButtonType				button;
			ButtonEvent				event;
		};

	public:
		BtnFunc() = default;
		BtnFunc(const T& input);
	
	public:
		template<typename Func>
		void Add(ButtonType button, ButtonEvent event, Func&& func);

		template<Enum Bind, typename Func>
		void Add(Bind key, ButtonEvent event, Func&& func);

		template<typename Func, class U>
		void Add(ButtonType button, ButtonEvent event, Func&& func, U* object);

		template<Enum Bind, typename Func, class U>
		void Add(Bind key, ButtonEvent event, Func&& func, U* object);

	public:
		void Execute();
		void operator()();

		void Execute(const T& input);
		void operator()(const T& input);

	private:
		std::vector<BoundFunc> m_funcs;
		const T* m_input {nullptr};
	};

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	inline BtnFunc<T, ButtonType>::BtnFunc(const T& input) : m_input(&input) {}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	template<typename Func>
	inline void BtnFunc<T, ButtonType>::Add(ButtonType button, ButtonEvent event, Func&& func)
	{
		m_funcs.emplace_back(std::forward<Func>(func), button, event);
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	template<Enum Bind, typename Func>
	inline void BtnFunc<T, ButtonType>::Add(Bind bind, ButtonEvent event, Func&& func)
	{
		const auto& binds = m_input->GetMap<Bind>();
		Add(binds.At(bind), event, std::forward<Func>(func));
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	template<typename Func, class U>
	inline void BtnFunc<T, ButtonType>::Add(ButtonType button, ButtonEvent event, Func&& func, U* object)
	{
		m_funcs.emplace_back(std::bind(std::forward<Func>(func), object), button, event);
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	template<Enum Bind, typename Func, class U>
	inline void BtnFunc<T, ButtonType>::Add(Bind key, ButtonEvent event, Func&& func, U* object)
	{
		const auto& binds = m_input->GetMap<Bind>();
		Add(binds.At(key), event, std::forward<Func>(func), object);
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	inline void BtnFunc<T, ButtonType>::Execute()
	{
		if (m_input != nullptr)
			Execute(*m_input);
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	inline void BtnFunc<T, ButtonType>::operator()()
	{
		Execute();
	}
	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	inline void BtnFunc<T, ButtonType>::Execute(const T& input)
	{
		for (const BoundFunc& bind : m_funcs)
		{
			bool triggered = false;

			switch (bind.event)
			{
			case BE_Pressed:	triggered = input.Pressed(bind.button);		break;
			case BE_Released:	triggered = input.Released(bind.button);	break;
			case BE_Held:		triggered = input.Held(bind.button);		break;
			}

			if (triggered)
			{
				if (bind.func)
					bind.func();
			}
		}
	}
	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	inline void BtnFunc<T, ButtonType>::operator()(const T& input)
	{
		Execute(input);
	}
}