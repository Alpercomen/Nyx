#pragma once

#include <vector>
#include <Application/Resource/Components/Components.h>
#include <Application/Resource/Components/Transform/Transform.h>
#include <Application/Core/Core.h>
#include <Application/Core/Physics/Types.h>

double GravitationalForce(double mu, double r);

double CalculateOrbitalVelocity(double otherMass, double r);

double RotationDegreeToLinearVelocity(float degreesPerSecond, float radiusMeters);

void InitializeCircularOrbit(EntityID satelliteID, EntityID attractorID, float32 inclination);

void Attract(const EntityID& objID);

void ApplyTidalLock(Transform& Ta, Transform& Tb, Rigidbody& Ra, Rigidbody& Rb);

void ComputeStrongestAttractors(Vector<SimBody>& bodies);

void ComputeTimeStepParents(Vector<SimBody>& bodies);

void ComputeSOIRadii(Vector<SimBody>& bodies);

EntityID ComputeTimeStepParentID(const Vector<SimBody>& bodies, int32 selfIndex);

EntityID ComputeStrongestAttractorID(const Vector<SimBody>& bodies, int32 selfIndex);

EntityID ComputeOrbitalParentID(const Vector<SimBody>& bodies, int32 selfIndex);