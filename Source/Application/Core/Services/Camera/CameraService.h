#pragma once
#include <Application/Core/Core.h>

namespace Nyx
{
	class CameraService : public Singleton<CameraService>
	{
	public:
		EntityID targetEntity;
		float distance = 100.0f;
		float minimumDistance = distance;
		float yaw = 0.0f;
		float pitch = 0.0f;
		bool enabled = false;

		~CameraService() = default;
	};

}