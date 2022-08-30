#pragma once

#include <Velox/Utilities.hpp>
#include <unordered_map>
#include <memory>

namespace vlx
{
	class BindsBase
	{
	public:
		using Ptr = std::unique_ptr<BindsBase>;

	public:
		virtual ~BindsBase() = 0;

		constexpr bool GetEnabled() const noexcept
		{
			return m_enabled;
		}
		void SetEnabled(bool flag)
		{
			m_enabled = flag;
		}

	private:
		bool m_enabled{true}; // enabled as default
	};

	template<ArithEnum Bind, ArithEnum Reg>
	class Binds final : public BindsBase
	{
	public:
		Reg& operator[](const Bind name)
		{
			return m_binds[name];
		}
		const Reg& operator[](const Bind name) const
		{
			return const_cast<Binds<Bind, Reg>*>(this)->operator[](name);
		}

		Reg& At(Bind name)
		{
			return m_binds.at(name);
		}
		const Reg& At(Bind name) const
		{
			return const_cast<Binds<Bind, Reg>*>(this)->At(name);
		}

		////////////////////////////////////////////////////////////
		// Set the bind name to match to button
		////////////////////////////////////////////////////////////
		void Set(Bind name, Reg button)
		{
			m_binds[name] = button;
		}

		////////////////////////////////////////////////////////////
		// Remove the existing bind
		////////////////////////////////////////////////////////////
		void Remove(Bind name)
		{
			m_binds.erase(name);
		}

	protected:
		std::unordered_map<Bind, Reg> m_binds;
	};

	inline BindsBase::~BindsBase() = default;
}