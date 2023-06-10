#pragma once

#include <string>

#include <Velox/Config.hpp>

namespace vlx
{
	///	Used for common properties in objects, essential for many systems. 
	/// 
	class VELOX_API Object
	{
	public:
		const std::string& GetTag() const noexcept;

		bool GetActive() const noexcept;
		bool GetActiveSelf() const noexcept;

		void SetTag(const std::string& tag);
		void SetTag(std::string&& tag);

		void SetActive(bool flag);

	private:
		std::string	m_tag;
		bool		m_activeHierarchy	{true};
		bool		m_activeSelf		{true};
		bool		m_dirty				{false};
	};
}