#include "PS4FrameBuffer.h"


PS4FrameBuffer::~PS4FrameBuffer()
{
}

PS4FrameBuffer::PS4FrameBuffer(PS4TextureNew * colourTex, PS4TextureNew * depthTex)
{
	//create the render targets and store the pointers to the texture for overriding later
	sce::Gnm::RenderTarget first;
	colourTarget.push_back(first);
	colourTarget.at(0).initFromTexture(&colourTex->apiTexture, 0);

	colourTexture.push_back(colourTex);
	
	hasColor = true;

	depthTarget.initFromTexture(&depthTex->apiTexture, 0);
	depthTexture = depthTex;

	hasDepth = true;

	height = colourTexture.at(0)->apiTexture.getHeight();
	width = colourTexture.at(0)->apiTexture.getWidth();
}


PS4FrameBuffer::PS4FrameBuffer(std::vector<PS4TextureNew*> colourTex, PS4TextureNew * depthTex)
{
	//create the render targets and store the pointers to the texture for overriding later
	for (uint i = 0; i < colourTex.size();++i) {
		sce::Gnm::RenderTarget n;
		colourTarget.push_back(n);
		colourTarget.at(i).initFromTexture(&colourTex.at(i)->apiTexture, 0);

		colourTexture.push_back(colourTex.at(i));
	}
	

	hasColor = true;

	depthTarget.initFromTexture(&depthTex->apiTexture, 0);
	depthTexture = depthTex;

	hasDepth = true;

	height = colourTexture.at(0)->apiTexture.getHeight();
	width = colourTexture.at(0)->apiTexture.getWidth();
}



PS4FrameBuffer::PS4FrameBuffer(PS4TextureNew * depthTex, bool colour)
{
	if (colour) {
	//should never get here
		hasColor = true;

	}
	
	depthTarget.initFromTexture(&depthTex->apiTexture, 0);
	depthTexture = depthTex;

	hasDepth = true;

	height = depthTexture->apiTexture.getHeight();
	width = depthTexture->apiTexture.getWidth();

}

void PS4FrameBuffer::SetMem(void *colourMemory, void *depthMemory) {
	if (hasColor) {
		this->colourTarget.at(0).setAddresses(colourMemory, NULL, NULL);
	}
	if (hasDepth) {
		this->depthTarget.setAddresses(depthMemory, NULL);
	}
}


void PS4FrameBuffer::SetMem(std::vector<void*>colourMemory, void *depthMemory) {
	if (hasColor) {
		for (uint i = 0; i < colourTarget.size(); ++i) {
			this->colourTarget.at(i).setAddresses(colourMemory.at(i), NULL, NULL);
		}
	}
	if (hasDepth) {
		this->depthTarget.setAddresses(depthMemory, NULL);
	}
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
		if (hasColor) {
			currentGFXContext->setRenderTarget(0, &this->colourTarget.at(0));
			currentGFXContext->setupScreenViewport(0, 0, this->colourTarget.at(0).getWidth(), this->colourTarget.at(0).getHeight(), 0.5f, 0.5f);
			currentGFXContext->setScreenScissor(0, 0, this->colourTarget.at(0).getWidth(), this->colourTarget.at(0).getHeight());
			currentGFXContext->setWindowScissor(0, 0, this->colourTarget.at(0).getWidth(), this->colourTarget.at(0).getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
			currentGFXContext->setGenericScissor(0, 0, this->colourTarget.at(0).getWidth(), this->colourTarget.at(0).getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
			if (hasDepth) {
				currentGFXContext->setDepthRenderTarget(&this->depthTarget);
			}
		}
		else {
			if (hasDepth) {
				currentGFXContext->setDepthRenderTarget(&this->depthTarget);
			
				currentGFXContext->setupScreenViewport(0, 0, this->depthTarget.getWidth(), this->depthTarget.getHeight(), 0.5f, 0.5f);
				currentGFXContext->setScreenScissor(0, 0, this->depthTarget.getWidth(), this->depthTarget.getHeight());
				currentGFXContext->setWindowScissor(0, 0, this->depthTarget.getWidth(), this->depthTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
				currentGFXContext->setGenericScissor(0, 0, this->depthTarget.getWidth(), this->depthTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
			}
		}
		ClearBuffer();
	}
}

void PS4FrameBuffer::ClearBuffer()
{
	//clear color
	if (hasColor) {
		Vector4 defaultClearColour(0.1f, 0.1f, 0.1f, 1.0f);
		SurfaceUtil::clearRenderTarget(*currentGFXContext, &this->colourTarget.at(0), defaultClearColour);
	}
	//clear depth 
	if (hasDepth) {
		float defaultDepth = 1.0f;
		SurfaceUtil::clearDepthTarget(*currentGFXContext, &this->depthTarget, defaultDepth);
	}
	
}
