#pragma once

#include <memory>
#include <functional>

#include <Velox/Utility/NonCopyable.h>

namespace vlx
{
	/// WeakEvent that ensures that it can only be executed if the associated object is still valid
	/// 
	class WeakEvent final : private NonCopyable
	{
	public:
		template<class Func, class T>
		static std::function<bool()> Weak(Func&& func, const std::shared_ptr<T>& obj) 
		{
			return std::bind(Wrapper(), std::weak_ptr<T>(obj), std::function<void()>(std::bind(std::forward<Func>(func), obj.get())));
		}

		struct Wrapper
		{
			template<class T>
			bool operator()(const std::weak_ptr<T>& obj, std::function<void()>& func) 
			{
				if (!obj.expired())
				{
					func();
					return true;
				}

				return false;
			}
		};
	};
}

