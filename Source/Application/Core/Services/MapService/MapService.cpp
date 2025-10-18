#pragma once
#include <QNetworkAccessManager>

#include "MapService.h"

namespace Nyx
{
	void MapService::Initialize(const String& url)
	{
		sourceUrl = url;
		//fetchCloudMap();
	}

	void MapService::Update()
	{
		// double now = Time::Now();
	}

	uint32 MapService::GetGLTextureID() const
	{
		return uint32();
	}

	void MapService::fetchMap()
	{

	}

	void MapService::uploadToGpu(const Vector<uint8> data)
	{

	}
}