#pragma once
#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
	class CameraService : public Singleton<CameraService>
	{
	public:
		EntityID targetEntity;
		float64 focusRadius = CAMERA_FOCUS_RADIUS; // Objects smaller than this value, will cause the camera to enter focus mode when tracking for better viewing.
		float64 minimumDistance = CAMERA_DEFAULT_MINIMUM_ZOOM;
		float64 maximumDistance = CAMERA_DEFAULT_MAXIMUM_ZOOM;

		float64 distance = 2.0;
		float64 yaw = 0.0;
		float64 pitch = 0.0;

		bool focusEnabled = false;
		bool enabled = false;
		bool lockOrientation = false;

		Math::Vec3d lockedFront = Math::Vec3d(0.0, 0.0, -1.0);
		Math::Vec3d lockedRight = Math::Vec3d(1.0, 0.0, 0.0);
		Math::Vec3d lockedUp = Math::Vec3d(0.0, 1.0, 0.0);

		void LockOn(EntityID& id)
		{
			auto& transform = *ECS::Get().GetComponent<Transform>(id);

			const auto& pos = transform.position.GetWorld();
			const auto& rot = transform.rotation.GetEulerAngles();
			const auto& sca = transform.scale.get();

			targetEntity = id;

			const double targetSize = glm::length(sca) / METERS_PER_UNIT;
			const double visualSize = glm::max(targetSize, 1.0);

			if (visualSize <= focusRadius)
				focusEnabled = true;
			else
				focusEnabled = false;
		}

		void Reset()
		{
			focusEnabled = false;
			targetEntity = NO_ID;
		}

		~CameraService() = default;
	};

}