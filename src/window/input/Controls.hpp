#pragma once

#include "../../utilities/NonCopyable.h"
#include "InputHandler.hpp"

namespace fge
{
	enum class Input : uint32_t
	{
		Keyboard,
		Mouse,
		Joystick,

		Count // number of input types
	};

	////////////////////////////////////////////////////////////
	// Neat wrapper around controls to prevent having to
	// cast all the time to access each controller
	////////////////////////////////////////////////////////////
	class Controls final : NonCopyable
	{
	public:
		Controls() : m_controls(static_cast<uint32_t>(Input::Count)) { }

		////////////////////////////////////////////////////////////
		// Gets the controller through comparing the type, a bit
		// slower than providing input type but looks nicer :)
		////////////////////////////////////////////////////////////
		template<class T> // unfortunately we cannot check if T derives from InputHandler, will have to create custom trait for that
		T& Get()
		{
			for (auto& control : m_controls)
				if (control.get())
				{
					if (typeid(*control.get()) == typeid(T))
						return *static_cast<T*>(control.get());
				}

			throw std::runtime_error("object of type T could not be found");
		}
		template<class T>
		const T& Get() const
		{
			return const_cast<Controls*>(this)->Get();
		}

		template<class T>
		T& Get(const Input& input)
		{
			return *static_cast<T*>(m_controls.at(static_cast<uint32_t>(input)).get());
		}
		template<class T>
		const T& Get(const Input& input) const
		{
			return const_cast<Controls*>(this)->Get(input); // static cast of T assumes that T and input matches
		}

		template<class T, typename... Args>
		void Add(const Input& input, Args&&... args)
		{
			const uint32_t index = static_cast<uint32_t>(input);

			if (m_controls.at(index).get())
				throw std::runtime_error("There is already an object defined at index: " + index);

			m_controls[index] = InputHandler::Ptr(new T(std::forward<Args>(args)...));
		}

		void Update(const Time& time)
		{
			for (auto& control : m_controls)
				if (control.get()) // check if defined
					control->Update(time);
		}
		void HandleEvent(const sf::Event& event)
		{
			for (auto& control : m_controls)
				if (control.get())
					control->HandleEvent(event);
		}

	private:
		InputHandler::Controls m_controls;
	};
}