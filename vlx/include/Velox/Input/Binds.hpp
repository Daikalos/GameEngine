#pragma once

#include <unordered_map>
#include <memory>

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

namespace vlx
{
	class IBinds
	{
	public:
		using Ptr = std::unique_ptr<IBinds>;

	public:
		virtual ~IBinds() = default;

		virtual bool GetEnabled() const noexcept = 0;
		virtual void SetEnabled(const bool flag) noexcept = 0;
	};

	template<ArithEnum Bind, ArithEnum Reg>
	class Binds final : public IBinds
	{
	public:
		NODISC Reg& operator[](const Bind name);
		NODISC const Reg& operator[](const Bind name) const;

		NODISC Reg& At(const Bind name);
		NODISC const Reg& At(const Bind name) const;

		/// <summary>
		///		Set the bind name to match to button
		/// </summary>
		void Set(const Bind name, const Reg button);

		/// <summary>
		///		Remove the existing bind
		/// </summary>
		void Remove(const Bind name);

	public:
		NODISC bool GetEnabled() const noexcept;
		void SetEnabled(const bool flag) noexcept;

	protected:
		std::unordered_map<Bind, Reg> m_binds;
		bool m_enabled	{true}; // enabled as default
	};

	template<ArithEnum Bind, ArithEnum Reg>
	inline Reg& Binds<Bind, Reg>::operator[](const Bind name)
	{
		return m_binds[name];
	}

	template<ArithEnum Bind, ArithEnum Reg>
	inline const Reg& Binds<Bind, Reg>::operator[](const Bind name) const
	{
		return m_binds[name];
	}

	template<ArithEnum Bind, ArithEnum Reg>
	inline Reg& Binds<Bind, Reg>::At(Bind name)
	{
		return const_cast<Reg&>(std::as_const(*this).At(name));;
	}

	template<ArithEnum Bind, ArithEnum Reg>
	inline const Reg& Binds<Bind, Reg>::At(Bind name) const
	{
		auto it = m_binds.find(name);

		if (it == m_binds.end())
			throw std::runtime_error("value could not be found");

		return it->second;
	}

	template<ArithEnum Bind, ArithEnum Reg>
	inline void Binds<Bind, Reg>::Set(Bind name, Reg button)
	{
		m_binds[name] = button;
	}

	template<ArithEnum Bind, ArithEnum Reg>
	inline void Binds<Bind, Reg>::Remove(Bind name)
	{
		m_binds.erase(name);
	}

	template<ArithEnum Bind, ArithEnum Reg>
	inline bool Binds<Bind, Reg>::GetEnabled() const noexcept
	{
		return m_enabled;
	}
	template<ArithEnum Bind, ArithEnum Reg>
	inline void Binds<Bind, Reg>::SetEnabled(const bool flag) noexcept
	{
		m_enabled = flag;
	}
}