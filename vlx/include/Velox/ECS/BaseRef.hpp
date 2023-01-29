#pragma once

#include "EntityAdmin.h"
#include "Identifiers.hpp"
#include "IComponentRef.hpp"

namespace vlx
{
	template<class B>
	class BaseRef final : public IComponentRef
	{
	public:
		BaseRef(const EntityID entity_id, const std::uint32_t offset = 0);

	public:
		B* operator->();
		const B* operator->() const;

		B& operator*();
		const B& operator*() const;

	public:
		B* Get();
		const B* Get() const;

	private:
		void Update(const EntityAdmin& entity_admin, void* component_data) override;

	private:
		B*				m_base_component		{nullptr};
		std::uint32_t	m_offset				{0};
	};

	template<class B>
	inline BaseRef<B>::BaseRef(const EntityID entity_id, const std::uint32_t offset)
		: IComponentRef(entity_id), m_base_component(nullptr), m_offset(offset) { }

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
		return m_base_component;
	}

	template<class B>
	inline const B* BaseRef<B>::Get() const
	{
		return const_cast<BaseRef<B>&>(*this).Get();
	}
}