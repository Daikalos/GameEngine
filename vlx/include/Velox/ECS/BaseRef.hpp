#pragma once

#include "EntityAdmin.h"
#include "Identifiers.hpp"

namespace vlx
{
	template<class B>
	class BaseRef final
	{
	public:
		BaseRef() = default;
		BaseRef(const EntityID entity_id, std::shared_ptr<void*> base_component);

	public:
		B* operator->();
		const B* operator->() const;

		B& operator*();
		const B& operator*() const;

	public:
		B* Get();
		const B* Get() const;

	public:
		EntityID GetEntityID() const noexcept;
		bool IsValid() const noexcept;

	public:
		void Reset();

	private:
		EntityID				m_entity_id			{NULL_ENTITY};
		std::shared_ptr<void*>	m_base_component	{nullptr};
	};

	template<class B>
	inline BaseRef<B>::BaseRef(const EntityID entity_id, std::shared_ptr<void*> base_component)
		: m_entity_id(entity_id), m_base_component(base_component) { }

	template<class B>
	inline B* BaseRef<B>::operator->()
	{
		return Get();
	}

	template<class B>
	inline const B* BaseRef<B>::operator->() const
	{
		return Get();
	}

	template<class B>
	inline B& BaseRef<B>::operator*()
	{
		return *Get();
	}

	template<class B>
	inline const B& BaseRef<B>::operator*() const
	{
		return *Get();
	}

	template<class B>
	inline B* BaseRef<B>::Get()
	{
		return static_cast<B*>(*m_base_component);
	}

	template<class B>
	inline const B* BaseRef<B>::Get() const
	{
		return const_cast<BaseRef<B>&>(*this).Get();
	}

	template<class B>
	inline EntityID BaseRef<B>::GetEntityID() const noexcept
	{
		return m_entity_id;
	}

	template<class B>
	inline bool BaseRef<B>::IsValid() const noexcept
	{
		return m_base_component != nullptr;
	}

	template<class B>
	inline void BaseRef<B>::Reset()
	{
		m_base_component.reset();
	}
}