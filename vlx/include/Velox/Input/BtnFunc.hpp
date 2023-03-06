#pragma once

#include <functional>
#include <array>

#include "InputHandler.h"

namespace vlx
{
	enum ButtonEvent : std::uint8_t
	{
		BE_Begin	= 0,

		BE_Pressed	= 0,
		BE_Released = 1,
		BE_Held		= 2,

		BE_End		= 3
	};

	inline ButtonEvent& operator++(ButtonEvent& button_event)
	{
		button_event = static_cast<ButtonEvent>(static_cast<std::uint8_t>(button_event) + 1);
		return button_event;
	}

	///	BtnFunc is small container class for mapping functions to individual key events. This was created to 
	/// have the same lifetime as the provided functions have in order to prevent common errors. It also allows 
	/// for the developer to determine when the functions should executed.
	/// 
	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	class BtnFunc final
	{
	public:
		using ButtonFuncs	= std::array<std::function<void()>, BE_End>;
		using BoundFunc		= std::pair<ButtonType, ButtonFuncs>;

	public:
		BtnFunc(const T& input);
	
	public:
		template<typename Func, class U>
		void Add(ButtonType button, ButtonEvent event, Func&& func, U* object);

		template<Enum Bind, typename Func, class U>
		void Add(Bind key, ButtonEvent event, Func&& func, U* object);

	public:
		void Execute();
		void operator()();

	private:
		const T* m_input {nullptr};
		std::vector<BoundFunc> m_funcs;
	};

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	inline BtnFunc<T, ButtonType>::BtnFunc(const T& input) : m_input(&input) {}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	template<typename Func, class U>
	inline void BtnFunc<T, ButtonType>::Add(ButtonType key, ButtonEvent event, Func&& func, U* object)
	{
		ButtonFuncs& funcs = m_funcs.emplace_back(key, ButtonFuncs()).second;
		funcs[event] = std::bind(std::forward<Func>(func), object);
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
		for (const auto& [key, button_func] : m_funcs)
		{
			for (ButtonEvent event = BE_Begin; event < BE_End; ++event)
			{
				bool triggered = false;

				switch (event)
				{
				case BE_Pressed:	triggered = m_input->Pressed(key);	break;
				case BE_Released:	triggered = m_input->Released(key);	break;
				case BE_Held:		triggered = m_input->Held(key);		break;
				}

				if (triggered)
				{
					const auto& func = button_func[event];
					if (func) func();
				}
			}
		}
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	inline void BtnFunc<T, ButtonType>::operator()()
	{
		Execute();
	}
}