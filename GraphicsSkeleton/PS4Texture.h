#pragma once
#include "TextureBase.h"
#include <string>
#include <gnm\texture.h>
#include "PS4MemoryAware.h"

class PS4Texture :
	public TextureBase, public PS4MemoryAware
{
public:
	friend class PS4RendererBase;

	static PS4Texture* LoadTextureFromFile(const std::string& filename);

	const sce::Gnm::Texture&  GetAPITexture() {
		return apiTexture;
	}

protected:
	PS4Texture();
	~PS4Texture();

	sce::Gnm::Texture apiTexture;

};

