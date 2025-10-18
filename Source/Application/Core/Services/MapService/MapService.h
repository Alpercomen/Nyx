#pragma once
#include <Application/Core/Core.h>

namespace Nyx
{
	class MapService : Singleton<MapService>
	{
	public:
		void Initialize(const String& url);
		void Update();
		uint32 GetGLTextureID() const;

	private:
		void fetchMap();
		void uploadToGpu(const Vector<uint8> data);

		String sourceUrl;
		uint32 textureID = 0;
		double lastFetchTime = 0.0;
		double refreshInterval = 10800.0; // 3 hours

	};
}