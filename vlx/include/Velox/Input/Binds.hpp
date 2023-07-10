#pragma once

#include <unordered_map>

#include <Velox/System/Concepts.h>
#include <Velox/Config.hpp>

namespace vlx
{
	template<typename Bind, typename Reg>
	class Binds
	{
	protected:
		Binds() = default;

	public:
		NODISC Reg& operator[](const Bind& name);
		NODISC const Reg& operator[](const Bind& name) const;

		NODISC Reg& At(const Bind& name);
		NODISC const Reg& At(const Bind& name) const;

		///	Set the bind name to match to button
		/// 
		void Set(const Bind& name, const Reg& button);

		///	Remove the existing bind
		/// 
		void Remove(const Bind& name);

	public:
		NODISC bool GetEnabled() const noexcept;
		void SetEnabled(bool flag) noexcept;

	protected:
		std::unordered_map<Bind, Reg> m_binds;
		bool m_enabled {true};
	};

	template<typename Bind, typename Reg>
	inline Reg& Binds<Bind, Reg>::operator[](const Bind& name)
	{
		return At(name);
	}

	template<typename Bind, typename Reg>
	inline const Reg& Binds<Bind, Reg>::operator[](const Bind& name) const
	{
		return At(name);
	}

	template<typename Bind, typename Reg>
	inline Reg& Binds<Bind, Reg>::At(const Bind& name)
	{
		return const_cast<Reg&>(std::as_const(*this).At(name));;
	}

	template<typename Bind, typename Reg>
	inline const Reg& Binds<Bind, Reg>::At(const Bind& name) const
	{
		const auto it = m_binds.find(name);
		if (it == m_binds.end())
			throw std::runtime_error("Bind could not be found");

		return it->second;
	}

	template<typename Bind, typename Reg>
	inline void Binds<Bind, Reg>::Set(const Bind& name, const Reg& button)
	{
		m_binds[name] = button;
	}

	template<typename Bind, typename Reg>
	inline void Binds<Bind, Reg>::Remove(const Bind& name)
	{
		const auto it = m_binds.find(name);
		if (it == m_binds.end())
			throw std::runtime_error("Bind could not be found");

		m_binds.erase(it);
	}

	template<typename Bind, typename Reg>
	inline bool Binds<Bind, Reg>::GetEnabled() const noexcept
	{
		return m_enabled;
	}
	template<typename Bind, typename Reg>
	inline void Binds<Bind, Reg>::SetEnabled(bool flag) noexcept
	{
		m_enabled = flag;
	}
}