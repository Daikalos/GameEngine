#pragma once

#include "Identifiers.hpp"
#include "IComponentRef.hpp"

namespace vlx
{
	class EntityAdmin;

	template<class B>
	class BaseRef final : public IComponentRef
	{
	public:
		BaseRef(const EntityAdmin& entity_admin, const EntityID entity_id, const ComponentTypeID child_component_id, const std::uint32_t offset = 0);

	public:
		B* operator->();
		const B* operator->() const;

		B& operator*();
		const B& operator*() const;

	public:
		operator bool() const noexcept;

	public:
		B* Get();
		const B* Get() const;

	public:
		[[nodiscard]] constexpr EntityID GetEntityID() const noexcept;

		[[nodiscard]] constexpr bool IsValid() const noexcept override;
		[[nodiscard]] constexpr bool IsExpired() const override;

	public:
		void Reset() override;
		void ForceUpdate() override;

	private:
		const EntityAdmin*	m_entity_admin			{nullptr};
		EntityID			m_entity_id				{NULL_ENTITY};

		ComponentTypeID		m_child_component_id	{0};
		std::uint32_t		m_offset				{0};

		B*					m_base_component		{nullptr};
		bool				m_expired				{false};
	};

	template<class B>
	inline BaseRef<B>::BaseRef(const EntityAdmin& entity_admin, const EntityID entity_id, const ComponentTypeID child_component_id, const std::uint32_t offset)
		: m_entity_admin(&entity_admin), m_entity_id(entity_id), m_child_component_id(child_component_id), m_offset(offset) { }

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
	inline BaseRef<B>::operator bool() const noexcept
	{
		return IsExpired();
	}

	template<class B>
	inline constexpr EntityID BaseRef<B>::GetEntityID() const noexcept
	{
		return m_entity_id;
	}

	template<class B>
	inline B* BaseRef<B>::Get()
	{
		ForceUpdate();
		return m_base_component;
	}

	template<class B>
	inline const B* BaseRef<B>::Get() const
	{
		return const_cast<BaseRef<B>&>(*this).Get();
	}

	template<class B>
	inline constexpr bool BaseRef<B>::IsValid() const noexcept
	{
		return m_base_component != nullptr;
	}

	template<class B>
	inline constexpr bool BaseRef<B>::IsExpired() const
	{
		return m_expired;
	}

	template<class B>
	inline void BaseRef<B>::Reset()
	{
		m_base_component = nullptr;
	}

	template<class B>
	inline void BaseRef<B>::ForceUpdate()
	{
		assert(m_entity_admin != nullptr && m_entity_id != NULL_ENTITY);

		if (!IsValid())
		{
			auto [base_component, success] = m_entity_admin->TryGetBase<B>(m_entity_id, m_child_component_id, m_offset);

			if (!success)
			{
				m_expired = true;
				return;
			}

			m_base_component = base_component;
			m_expired = false;
		}
	}
}