#pragma once

#include <vector>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Components/Transform/Transform.h>
#include <Application/Core/Core.h>

double GravitationalForce(double mu, double r);

double CalculateOrbitalVelocity(double otherMass, double r);

double RotationDegreeToLinearVelocity(float degreesPerSecond, float radiusMeters);

void InitializeCircularOrbit(EntityID satelliteID, EntityID attractorID, float32 inclination, bool isTidallyLocked = false);

void Attract();

void ApplyTidalLock(Transform& Ta, Transform& Tb, Rigidbody& Ra);