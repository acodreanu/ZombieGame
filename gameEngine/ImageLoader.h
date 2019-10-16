#pragma once
#include "GLTexture.h"
#include <string>
namespace gameEngine {
	class ImageLoader
	{
	public:
		static GLTexture loadPNG(std::string filePath);
	};
}

