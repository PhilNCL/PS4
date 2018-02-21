#include "PS4FrameBuffer.h"


void PS4FrameBuffer::InitialiseMemoryAllocators() {
}


PS4FrameBuffer::~PS4FrameBuffer()
{
}

PS4FrameBuffer::PS4FrameBuffer(PS4TextureNew * colourTex, PS4TextureNew * depthTex)
{
	//create the render targets and store the pointers to the texture for overriding later
	colourTarget.initFromTexture(&colourTex->apiTexture, 0);
	colourTexture = colourTex;


	depthTarget.initFromTexture(&depthTex->apiTexture, 0);
	depthTexture = depthTex;

	height = colourTexture->apiTexture.getHeight();
	width = colourTexture->apiTexture.getWidth();
}

void PS4FrameBuffer::SetMem(void *colourMemory, void *depthMemory) {

	this->colourTarget.setAddresses(colourMemory, NULL, NULL);

	this->depthTarget.setAddresses(depthMemory, NULL);
}


PS4FrameBuffer::PS4FrameBuffer(std::vector<PS4TextureNew*> colourTex, PS4TextureNew * depthTex)
{
}



PS4FrameBuffer::PS4FrameBuffer(PS4TextureNew * depthTex, bool colour)
{
}

uint PS4FrameBuffer::GetWidth()
{
	return width;
}

uint PS4FrameBuffer::GetHeight()
{
	return height;
}

void PS4FrameBuffer::Activate()
{
	if (currentGFXContext == nullptr) {
		return;
	}
	else {
		currentGFXContext->setRenderTargetMask(0xF);
		// Not every FBO has a CT/DT
		currentGFXContext->setRenderTarget(0, &this->colourTarget);
		currentGFXContext->setDepthRenderTarget(&this->depthTarget);

		currentGFXContext->setupScreenViewport(0, 0, this->colourTarget.getWidth(), this->colourTarget.getHeight(), 0.5f, 0.5f);
		currentGFXContext->setScreenScissor(0, 0, this->colourTarget.getWidth(), this->colourTarget.getHeight());
		currentGFXContext->setWindowScissor(0, 0, this->colourTarget.getWidth(), this->colourTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
		currentGFXContext->setGenericScissor(0, 0, this->colourTarget.getWidth(), this->colourTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);

		ClearBuffer();
	}
}

void PS4FrameBuffer::ClearBuffer()
{
	//clear color
		//Vector4 defaultClearColour(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1.0f);
		Vector4 defaultClearColour(0.1f, 0.1f, 0.1f, 1.0f);
		SurfaceUtil::clearRenderTarget(*currentGFXContext, &this->colourTarget, defaultClearColour);
	
	//clear depth 
	
		float defaultDepth = 1.0f;
		SurfaceUtil::clearDepthTarget(*currentGFXContext, &this->depthTarget, defaultDepth);
	
	//clear stencil
		if (this->depthTarget.getStencilReadAddress()) {
			int defaultStencil = 0;
			SurfaceUtil::clearStencilTarget(*currentGFXContext, &this->depthTarget, defaultStencil);
	    }
		
	

}
