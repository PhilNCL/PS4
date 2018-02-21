#pragma once

#include "common.h"
#include <string>

#include "PS4MemoryAware.h"

namespace Texture
{
	enum Type { COLOUR, DEPTH, DEPTH_ARRAY };
}

class PS4TextureNew : public PS4MemoryAware
{
public:
	PS4TextureNew();
	PS4TextureNew(const std::string& filepath);
	~PS4TextureNew();

	void SetCurrentGFXContext(sce::Gnmx::GnmxGfxContext* GFXContext);
	void Bind(int textureUnit = 0);
	//void SetTextureFiltering(bool nearest = false);
	//void SetTextureWrapping(bool repeating = false);
	sce::Gnm::Texture apiTexture;

protected:

	bool LoadTexture(const std::string& filepath);


	

	sce::Gnmx::GnmxGfxContext*	currentGFXContext = nullptr;

	sce::Gnm::Sampler trilinearSampler;

	//bool repeating;
	//bool 
};