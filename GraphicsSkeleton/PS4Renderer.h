#pragma once

#include <gnmx\context.h>
#include <gnm.h>

#include "PS4MemoryAware.h"

namespace Renderer
{
	enum Clear { COLOUR, DEPTH, COLOUR_DEPTH };
	enum Culling { FRONT, BACK };
};

//class Vector3;
class PS4ScreenBuffer;

class PS4Renderer : public PS4MemoryAware
{
public:
	PS4Renderer();
	~PS4Renderer();

	void	SetViewPort(int width, int height);
	void	Clear(Renderer::Clear clearType);
	//void	SetClearColour(Vector3& colour);

	void	BindScreenFramebuffer();


	void	SetScreenCulling(Renderer::Culling type);
	void	SetDefaultSettings();

protected:
	sce::Gnm::PrimitiveSetup	  primitiveSetup;
	sce::Gnm::BlendControl		  blendControlSetup;
	sce::Gnm::DepthStencilControl depthControlSetup;

	sce::Gnmx::GnmxGfxContext*	  currentGFXContext;
	sce::Gnm::RenderTarget*		  colourTarget;
	sce::Gnm::DepthRenderTarget*  depthTarget;
};

