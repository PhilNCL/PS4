#include "PS4Renderer.h"

using namespace Renderer;
using namespace sce;
using namespace Gnm;

#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h> // ClearRenderTarget()
#include <..\samples\sample_code\common\include\sampleutil\graphics\context.h> // SetDepthFunc()
#include <..\samples\sample_code\common\include\sampleutil\graphics\constant.h> // DepthFunc Enum

PS4Renderer::PS4Renderer()
{
	//InitialiseMemoryAllocators();
}


PS4Renderer::~PS4Renderer()
{
}

void	PS4Renderer::SetViewPort(int width, int height)
{
	if (currentGFXContext)
	{
		currentGFXContext->setupScreenViewport(0, 0, width, height, 0.5f, 0.5f);
	}
}
void	PS4Renderer::Clear(Renderer::Clear clearType)
{

}
//void	PS4Renderer::SetClearColour(Vector3& colour)
//{
//	if (currentGFXContext && colourTarget)
//	{
//		//TODO: Headache alert!
//		//sce::Vectormath::Scalar::Aos::Vector3 PScolour(colour.x, colour.y, colour.z);
//		//Gnmx::Toolkit::SurfaceUtil::clearRenderTarget(*currentGFXContext, colourTarget, PScolour);
//	}
//
//}

void	PS4Renderer::BindScreenFramebuffer()
{

}


void	PS4Renderer::SetScreenCulling(Renderer::Culling type)
{
	switch (type)
	{
	case FRONT:
		primitiveSetup.setCullFace(kPrimitiveSetupCullFaceFront);
		break;
	case BACK:
		primitiveSetup.setCullFace(kPrimitiveSetupCullFaceBack);
		break;
	}
}
void	PS4Renderer::SetDefaultSettings()
{
	// Turn on face culling
	primitiveSetup.init();
	primitiveSetup.setCullFace(kPrimitiveSetupCullFaceFront);
	primitiveSetup.setFrontFace(kPrimitiveSetupFrontFaceCcw);

	// Setup depth testing
	depthControlSetup.init();
	depthControlSetup.setDepthEnable(true);
	//depthControlSetup.setDepthControl(true, kDepthFuncLessEqual);
	if (currentGFXContext)
	{
		//SampleUtil::Graphics::setDepthFunc(SampleUtil::Graphics::kDepthFuncLessEqual);
	}

	// Set up alpha blending
	blendControlSetup.init();
	blendControlSetup.setBlendEnable(true);
	blendControlSetup.setAlphaEquation(kBlendMultiplierSrcAlpha, kBlendFuncAdd, kBlendMultiplierOneMinusSrcAlpha);


	// Not implemented 
	//glEnable(GL_FRAMEBUFFER_SRGB);
	//glEnable(GL_FRAMEBUFFER_SRGB);

}
