#pragma once
#include <Application/Core/Core.h>

namespace Nyx
{
	class SimulationControls : public Singleton<SimulationControls>
	{
	public:
		~SimulationControls() = default;

		bool8 GetIsPlaying() { return isPlaying; }
		int32 GetMinSpeed() { return minSpeed; }
		int32 GetMaxSpeed() { return maxSpeed; }
		int32 GetTimeDesired() { return timeDesired; }

		void SetIsPlaying(bool8 value) { isPlaying = value; }
		void SetTimeDesired(int32 value) { timeDesired = std::clamp(value, minSpeed, maxSpeed); }

	private:
		bool8 isPlaying = false;
		int32 minSpeed = 1;
		int32 maxSpeed = 1000000;
		int32 timeDesired = 1;
	};

}