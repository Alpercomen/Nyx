#pragma once

#include <Application/Core/Core.h>
#include <Application/Resource/Components/Components.h>

namespace Nyx
{
	struct AtmosphereDesc
	{
		Math::Vec3f scatteringColor = { 0.5f, 0.7f, 1.0f };
		float densityFalloff = 1.5f;
		float edgeStrength = 2.2f;
		float centerStrength = 0.35f;
		float edgePower = 3.0f;
		float centerPower = 3.0f;
		float exposure = 3.0f;
		float atmosphereThickness = 100.0f;
		Mesh* atmosphereMesh = nullptr;
	};

	class Atmosphere
	{
	public:
		AtmosphereDesc m_atmosphereDesc;
		Material m_atmosphereMaterial;

		Atmosphere() : Atmosphere(AtmosphereDesc()) {}
		Atmosphere(AtmosphereDesc atmosphereDesc) : m_atmosphereDesc(atmosphereDesc)
		{
			Shader atmosphereShader = ResourceManager::GetShader(
				"AtmosphereShader",
				R"(Nyx\Source\Application\Shaders\Atmosphere\atmosphere.vert)",
				R"(Nyx\Source\Application\Shaders\Atmosphere\atmosphere.frag)"
			);

			m_atmosphereMaterial = Material(
				atmosphereShader,
				{ 1.0f, 1.0f, 1.0f },
				{ 0.0f, 0.0f, 0.0f },
				0.0f
			);
		}
	};
}