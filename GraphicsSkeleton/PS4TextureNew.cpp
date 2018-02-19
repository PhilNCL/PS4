#include "PS4TextureNew.h"

#include <fstream>

#include <gnf.h>

PS4TextureNew::PS4TextureNew(const std::string& filepath)
{
	LoadTexture(filepath);
	//if (loadSuccess)
	//{
		//SetTextureFiltering();
		//SetTextureWrapping();
	//}

	trilinearSampler.init();
	trilinearSampler.setMipFilterMode(sce::Gnm::kMipFilterModeLinear);
}


PS4TextureNew::~PS4TextureNew()
{
}


bool PS4TextureNew::LoadTexture(const std::string& filepath)
{
	std::ifstream file(filepath, std::ios::binary);

	if (!file) {
		return false;
	}


	// Extract the Gnf header - contentsSize and magicNumber
	sce::Gnf::Header header;
	file.read((char*)&header, sizeof(header));

	// Is it a gnf file?
	if (header.m_magicNumber != sce::Gnf::kMagic) {
		return false;
	}

	// Load texture data
	char* rawContents = new char[header.m_contentsSize];
	file.read((char*)rawContents, header.m_contentsSize);

	// Allocate GPU and bus memory
	sce::Gnf::Contents* contentsDesc = (sce::Gnf::Contents*)rawContents;
	sce::Gnm::SizeAlign dataParams = getTexturePixelsSize(contentsDesc, 0);

	void *pixelsAddr = garlicAllocator.allocate(dataParams);
	sce::Gnm::registerResource(nullptr, ownerHandle, pixelsAddr, dataParams.m_size, filepath.c_str(), sce::Gnm::kResourceTypeTextureBaseAddress, 0);

	// Move to the start of the texture data (assume one texture per file)
	file.seekg(getTexturePixelsByteOffset(contentsDesc, 0), ios::cur); 
	file.read((char*)pixelsAddr, dataParams.m_size);

	apiTexture = *patchTextures(contentsDesc, 0, 1, &pixelsAddr);
	apiTexture.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);
	//width =	apiTexture.getWidth();
	//height = apiTexture.getHeight();

	file.close();
	delete[] rawContents;

	return true;
}

void PS4TextureNew::SetCurrentGFXContext(sce::Gnmx::GnmxGfxContext* GFXContext)
{

	currentGFXContext = GFXContext;
	currentGFXContext->setSamplers(sce::Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

}

void PS4TextureNew::Bind(int textureUnit)
{
	if (currentGFXContext)
	{
		currentGFXContext->setTextures(sce::Gnm::kShaderStagePs, 0, 1, &apiTexture);
	}
	
}



