#pragma once

#include <functional>
#include <vector>
#include <algorithm>

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
	template<class T, typename... Args> requires HasButtonInput<T>
	class ButtonEvent final
	{
	private:
		using ButtonType = typename T::ButtonType;

	private:
		struct ButtonCallback
		{
			std::function<void(Args...)>	func;
			ButtonType						button;				// button type to listen for
			ButtonTrigger					trigger;			// button trigger to listen for
			float							priority {0.0f};	// determines position in callbacks
		};

	public:
		ButtonEvent() = default;
		ButtonEvent(const T& input);

		ButtonEvent(const ButtonEvent& rhs);
		ButtonEvent(ButtonEvent&& rhs) noexcept;

		auto operator=(const ButtonEvent& rhs) -> ButtonEvent&;
		auto operator=(ButtonEvent&& rhs) noexcept -> ButtonEvent&;
	
	public:
		template<typename Func>
		void Add(ButtonType button, ButtonTrigger trigger, Func&& func, float priority = {})
		{
			cu::InsertSorted<ButtonCallback>(m_callbacks, ButtonCallback(std::forward<Func>(func), button, trigger, priority),
				[](const ButtonCallback& lhs, const ButtonCallback& rhs)
				{
					return lhs.priority > rhs.priority;
				});
		}

		template<typename Func, typename... InnerArgs>
		void Add(ButtonType button, ButtonTrigger trigger, float priority, Func&& func, InnerArgs&&... args)
		{
			Add(button, trigger, std::bind(std::forward<Func>(func), std::forward<InnerArgs>(args)...), priority);
		}

		template<typename Func, typename... InnerArgs>
		void Add(ButtonType button, ButtonTrigger trigger, Func&& func, InnerArgs&&... args)
		{
			Add(button, trigger, {}, std::forward<Func>(func), std::forward<InnerArgs>(args)...);
		}

		void Remove(ButtonType button);
		void Remove(ButtonTrigger trigger);

	public:
		std::size_t Size() const noexcept;

		void Execute(Args&&... args);
		void operator()(Args&&... args);

		void SetInput(const T* input);

		void Clear();

	private:
		std::vector<ButtonCallback> m_callbacks;
		const T* m_input {nullptr};
	};

	template<class T, typename... Args> requires HasButtonInput<T>
	inline ButtonEvent<T, Args...>::ButtonEvent(const T& input) 
		: m_input(&input) { }

	template<class T, typename... Args> requires HasButtonInput<T>
	inline ButtonEvent<T, Args...>::ButtonEvent(const ButtonEvent& rhs)
		: m_callbacks(rhs.m_callbacks), m_input(rhs.m_input)
	{

	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline ButtonEvent<T, Args...>::ButtonEvent(ButtonEvent&& rhs) noexcept
		: m_callbacks(std::move(rhs.m_callbacks)), m_input(std::move(rhs.m_input))
	{
		rhs.m_input = nullptr;
	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline auto ButtonEvent<T, Args...>::operator=(const ButtonEvent& rhs) -> ButtonEvent&
	{
		m_callbacks = rhs.m_callbacks;
		m_input = rhs.m_input;

		return *this;
	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline auto ButtonEvent<T, Args...>::operator=(ButtonEvent&& rhs) noexcept -> ButtonEvent&
	{
		m_callbacks = std::move(rhs.m_callbacks);

		m_input = rhs.m_input;
		rhs.m_input = nullptr;

		return *this;
	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline void ButtonEvent<T, Args...>::Remove(ButtonType button)
	{
		std::erase(std::remove_if(m_callbacks.begin(), m_callbacks.end(), 
			[&button](const ButtonCallback& callback)
			{
				return callback.button == button;
			}), m_callbacks.end());
	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline void ButtonEvent<T, Args...>::Remove(ButtonTrigger trigger)
	{
		std::erase(std::remove_if(m_callbacks.begin(), m_callbacks.end(),
			[&trigger](const ButtonCallback& callback)
			{
				return callback.trigger == trigger;
			}), m_callbacks.end());
	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline std::size_t ButtonEvent<T, Args...>::Size() const noexcept
	{
		return m_callbacks.size();
	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline void ButtonEvent<T, Args...>::Execute(Args&&... args)
	{
		if (m_input == nullptr)
			return;

		const auto CheckTriggered = [this](const ButtonCallback& callback) -> bool
		{
			switch (callback.trigger)
			{
			case BT_Pressed:	return m_input->Pressed(callback.button);
			case BT_Released:	return m_input->Released(callback.button);
			case BT_Held:		return m_input->Held(callback.button);
			default:			return false;
			}
		};

		for (const auto& callback : m_callbacks)
		{
			if (CheckTriggered(callback) && callback.func)
				callback.func(std::forward<Args>(args)...);
		}
	}
	template<class T, typename... Args> requires HasButtonInput<T>
	inline void ButtonEvent<T, Args...>::operator()(Args&&... args)
	{
		Execute(std::forward<Args>(args)...);
	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline void ButtonEvent<T, Args...>::SetInput(const T* input)
	{
		m_input = input;
	}

	template<class T, typename... Args> requires HasButtonInput<T>
	inline void ButtonEvent<T, Args...>::Clear()
	{
		m_callbacks.clear();
	}


}