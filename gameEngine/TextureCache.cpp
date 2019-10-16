#include "TextureCache.h"
#include "ImageLoader.h"
#include <iostream>

namespace gameEngine {
	TextureCache::TextureCache()
	{
	}


	TextureCache::~TextureCache()
	{
	}

	GLTexture TextureCache::getTexture(std::string texturePath) {
		//lookup the texture and see if its in the map
		auto mit = _textureMap.find(texturePath);

		//check if its not in the map
		if (mit == _textureMap.end()) {
			GLTexture newTexture = ImageLoader::loadPNG(texturePath);
			//Insert into the map
			_textureMap.insert(make_pair(texturePath, newTexture));

			//std::cout << "Loaded Texture!\n";
			return newTexture;
		}
		//std::cout << "Loaded Cached Texture!\n";
		return mit->second;
	}
}