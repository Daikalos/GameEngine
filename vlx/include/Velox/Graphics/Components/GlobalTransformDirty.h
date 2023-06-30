#pragma once

namespace vlx
{
	class GlobalTransformDirty
	{
	private:
		bool m_dirty			{true};
		bool m_update_position	{true};
		bool m_update_rotation	{true};
		bool m_update_scale		{true};

		friend class TransformSystem;
	};
}