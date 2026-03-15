#pragma once
#include <Application/Core/Core.h>

namespace Nyx
{
	class CameraService : public Singleton<CameraService>
	{
	public:
		EntityID targetEntity;
		float64 distance = 2.0;
		float64 focusRadius = CAMERA_FOCUS_RADIUS; // Objects smaller than this value, will cause the camera to enter focus mode when tracking for better viewing.
		float64 minimumDistance = CAMERA_DEFAULT_MINIMUM_ZOOM;
		float64 maximumDistance = CAMERA_DEFAULT_MAXIMUM_ZOOM;
		float64 yaw = 0.0;
		float64 pitch = 0.0;
		bool focusEnabled = false;
		bool enabled = false;

		~CameraService() = default;
	};

}