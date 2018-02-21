#pragma once
#include "PS4TextureNew.h"
#include <vector>
#include <gnm.h>
#include <gnmx\fetchshaderhelper.h>
#include "PS4MemoryAware.h"
#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>

using namespace sce;
using namespace Gnmx;
using namespace Gnmx::Toolkit;

class PS4FrameBuffer : public PS4MemoryAware
{
public:
	void InitialiseMemoryAllocators();
	~PS4FrameBuffer();

	PS4FrameBuffer(PS4TextureNew* colourTex, PS4TextureNew* depthTex);
	void SetMem(void * colourMemory, void * depthMemory);
	PS4FrameBuffer(std::vector<PS4TextureNew*> colourTex, PS4TextureNew* depthTex);
	PS4FrameBuffer(PS4TextureNew* depthTex, bool colour);

	uint GetWidth();
	uint GetHeight();

	void Activate();


	inline void SetGraphicsContext(sce::Gnmx::GnmxGfxContext* cmdList) { currentGFXContext = cmdList; }

	void ClearBuffer();

	uint bufferID;


	sce::Gnm::RenderTarget		colourTarget;
	sce::Gnm::DepthRenderTarget depthTarget;
protected:

	uint width;
	uint height;


	PS4TextureNew* colourTexture;
	PS4TextureNew* depthTexture;

	// Current graphics context
	sce::Gnmx::GnmxGfxContext*	   currentGFXContext = nullptr;
};

