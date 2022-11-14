#pragma once

namespace vlx::gui
{
	class Selectable
	{
	public:
		bool Selected() const noexcept;

	private:
		bool m_selected{false};
	};
}