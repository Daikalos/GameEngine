#pragma once

#include <functional>
#include <array>

#include <Velox/Utility/ContainerUtils.h>

#include "InputHandler.h"

namespace vlx
{
	enum ButtonEvent : uint8
	{
		BE_Pressed,
		BE_Released,
		BE_Held,
	};

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
			float					priority {0.0f};

			bool operator==(const BoundFunc& rhs) const
			{
				return button == rhs.button && event == rhs.event && priority == rhs.priority;
			}

			bool operator!=(const BoundFunc& rhs) const
			{
				return !(*this == rhs);
			}
		};

	public:
		BtnFunc() = default;
		BtnFunc(const T& input);
	
	public:
		template<typename Func>
		void Add(ButtonType button, ButtonEvent event, Func&& func, float priority = 0.0f);

		template<Enum Bind, typename Func>
		void Add(Bind key, ButtonEvent event, Func&& func, float priority = 0.0f);

		template<typename Func, class U>
		void Add(ButtonType button, ButtonEvent event, Func&& func, U* object, float priority = 0.0f);

		template<Enum Bind, typename Func, class U>
		void Add(Bind key, ButtonEvent event, Func&& func, U* object, float priority = 0.0f);

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
	inline void BtnFunc<T, ButtonType>::Add(ButtonType button, ButtonEvent event, Func&& func, float priority)
	{
		cu::InsertSorted(m_funcs, BoundFunc(std::forward<Func>(func), button, event, priority),
			[](const BoundFunc& lhs, const BoundFunc& rhs)
			{
				return lhs.priority > rhs.priority;
			});
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	template<Enum Bind, typename Func>
	inline void BtnFunc<T, ButtonType>::Add(Bind bind, ButtonEvent event, Func&& func, float priority)
	{
		const auto& binds = m_input->GetMap<Bind>();
		Add(binds.At(bind), event, std::forward<Func>(func), priority);
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	template<typename Func, class U>
	inline void BtnFunc<T, ButtonType>::Add(ButtonType button, ButtonEvent event, Func&& func, U* object, float priority)
	{
		Add(button, event, std::bind(std::forward<Func>(func), object), priority);
	}

	template<std::derived_from<InputHandler> T, Enum ButtonType> requires IsButtonInput<T, ButtonType>
	template<Enum Bind, typename Func, class U>
	inline void BtnFunc<T, ButtonType>::Add(Bind key, ButtonEvent event, Func&& func, U* object, float priority)
	{
		const auto& binds = m_input->GetMap<Bind>();
		Add(binds.At(key), event, std::forward<Func>(func), object, priority);
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