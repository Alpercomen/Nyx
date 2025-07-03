#pragma once

#include <Application/Constants/Constants.h>

class Meter {
public:
	// Getters
	const double GetWorld() const { return world; }
	const double GetNormal() const { return normalized; }

	// Setters
	void SetWorld(const double& meter)
	{
		world = meter;
		normalized = meter / METERS_PER_UNIT;
	}

	void SetNormal(const double& meter)
	{
		world = meter * METERS_PER_UNIT;
		normalized = meter;
	}

private:
	double world;
	double normalized;
};