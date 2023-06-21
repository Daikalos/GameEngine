#pragma once

#include <memory>
#include <span>

#include <Velox/ECS/EntityAdmin.h>
#include <Velox/ECS/Identifiers.hpp>

#include <Velox/Config.hpp>

namespace vlx
{
	class Window;
	class SystemBase;

	/// Interface for grouping similar systems to define specific behaviour
	/// 
	class VELOX_API SystemAction
	{
	public:
		using Ptr = std::unique_ptr<SystemAction>;

	public:
		SystemAction(EntityAdmin& entity_admin, LayerType id, bool required = false);
		virtual ~SystemAction() = 0;

	public:
		/// Returns the layer this system is associated with
		///
		NODISC LayerType GetLayer() const noexcept;

		/// Returns the layer this system is associated with
		///
		NODISC bool GetEnabled() const noexcept;

		/// Determines if this system is able of being removed from the world
		///
		NODISC bool IsRequired() const noexcept;

	public:
		void SetEnabled(bool flag);

	public:
		virtual void Start()		{};

		virtual void PreUpdate()	{};
		virtual void Update()		{};
		virtual void FixedUpdate()	{};
		virtual void PostUpdate()	{};

		virtual void Draw(Window& window) const		{};
		virtual void DrawGUI(Window& window) const	{};

	protected:
		/// Executes all systems located in the layer this is associated with
		///
		void Execute() const;

		/// Executes the specified system
		///
		void Execute(const SystemBase& system);

		/// Executes the specified systems
		///
		void Execute(std::span<const SystemBase*> systems);

	protected:
		EntityAdmin*	m_entity_admin	{nullptr};

	private:
		LayerType		m_layer			{LYR_NONE};
		bool			m_required		{false};
		bool			m_enabled		{true};
	};

	inline SystemAction::~SystemAction() = default;
}