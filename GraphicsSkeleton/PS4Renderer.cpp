#include "PS4Renderer.h"

using namespace Renderer;
using namespace sce;
using namespace Gnm;

#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h> // ClearRenderTarget()
#include <..\samples\sample_code\common\include\sampleutil\graphics\context.h> // SetDepthFunc()
#include <..\samples\sample_code\common\include\sampleutil\graphics\constant.h> // DepthFunc Enum

#include <video_out.h>	//Video System

#include "PS4FrameBuffer.h"
#include "PS4Frame.h"
#include "PS4ShaderNew.h"
#include "PS4TextureNew.h"
#include "PS4RenderNode.h"

#include "nclPS4Interface.h"

struct PS4ScreenBuffer {
	sce::Gnm::RenderTarget		colourTarget;
	sce::Gnm::DepthRenderTarget depthTarget;
};

PS4Renderer::PS4Renderer() :
	numBuffers(3),
	maxCMDBuffers(3),
	garlicMemoryBytes(1024 * 1024 * 512),
	onionMemoryBytes(1024 * 1024 * 256)
{
	framesSubmitted = 0;
	currentGPUBuffer = 0;
	currentScreenBuffer = 0;
	prevScreenBuffer = 0;

	currentGFXContext = nullptr;
	InitialiseMemoryAllocators();
	InitialiseGCMRendering();
	InitialiseVideoSystem();

	SwapBuffers();
}


PS4Renderer::~PS4Renderer()
{
	DestroyGCMRendering();
	DestroyVideoSystem();
	DestroyMemoryAllocators();
}

// What about the command buffer?
void	PS4Renderer::SwapBuffers()
{
	SwapScreenBuffer();
	SwapCommandBuffer();
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

void	PS4Renderer::InitialiseVideoSystem() {
	FrameBuffers = new PS4FrameBuffer*[numBuffers];

	for (int i = 0; i < numBuffers; ++i) {
		FrameBuffers[i] = GenerateScreenFrameBuffer(1920, 1080);
	}

	//Now we can open up the video port
	videoHandle = sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);

	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute,
		SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
		SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		FrameBuffers[0]->colourTarget.at(0).getWidth(),
		FrameBuffers[0]->colourTarget.at(0).getHeight(),
		FrameBuffers[0]->colourTarget.at(0).getPitch()
	);

	void* bufferAddresses[numBuffers];

	for (int i = 0; i < numBuffers; ++i) {
		bufferAddresses[i] = FrameBuffers[i]->colourTarget.at(0).getBaseAddress();
	}

	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, numBuffers, &attribute);
}


void	PS4Renderer::InitialiseMemoryAllocators() {
	stackAllocators[GARLIC].init(SCE_KERNEL_WC_GARLIC, garlicMemoryBytes);
	stackAllocators[ONION].init(SCE_KERNEL_WB_ONION, onionMemoryBytes);

	this->garlicAllocator = Gnmx::Toolkit::GetInterface(&stackAllocators[GARLIC]);
	this->onionAllocator = Gnmx::Toolkit::GetInterface(&stackAllocators[ONION]);
	Gnm::registerOwner(&ownerHandle, "PS4Renderer");
}


PS4FrameBuffer*	PS4Renderer::GenerateScreenFrameBuffer(uint width, uint height, bool colour, bool depth, bool stencil)
{

	PS4ScreenBuffer* buffer = new PS4ScreenBuffer();

	//create color 
	Gnm::RenderTargetSpec spec;
	spec.init();
	spec.m_width = width;
	spec.m_height = height;
	spec.m_numSamples = Gnm::kNumSamples1;
	spec.m_numFragments = Gnm::kNumFragments1;
	spec.m_colorFormat = Gnm::kDataFormatB8G8R8A8UnormSrgb;

	GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec.m_colorTileModeHint, GpuAddress::kSurfaceTypeColorTargetDisplayable, spec.m_colorFormat, 1);

	int32_t success = buffer->colourTarget.init(&spec);

	if (success != SCE_GNM_OK) {
		bool a = true;
	}

	const Gnm::SizeAlign colourAlign = buffer->colourTarget.getColorSizeAlign();

	void *colourMemory = stackAllocators[GARLIC].allocate(colourAlign);

	Gnm::registerResource(nullptr, ownerHandle, colourMemory, colourAlign.m_size,
		"Colour", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

	//create depth
	Gnm::DepthRenderTargetSpec spec2;
	spec2.init();
	spec2.m_width = width;
	spec2.m_height = height;
	spec2.m_numFragments = Gnm::kNumFragments1;
	spec2.m_zFormat = Gnm::ZFormat::kZFormat32Float;
	spec2.m_stencilFormat = (stencil ? Gnm::kStencil8 : Gnm::kStencilInvalid);

	GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec2.m_tileModeHint, GpuAddress::kSurfaceTypeDepthTarget, Gnm::DataFormat::build(spec2.m_zFormat), 1);

	void* stencilMemory = 0;

	int32_t success2 = buffer->depthTarget.init(&spec2);

	if (success2 != SCE_GNM_OK) {
		bool a = true;
	}

	void *depthMemory = stackAllocators[GARLIC].allocate(buffer->depthTarget.getZSizeAlign());

	Gnm::registerResource(nullptr, ownerHandle, depthMemory, buffer->depthTarget.getZSizeAlign().m_size,
		"Depth", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);



	Gnm::Texture colorT;
	colorT.initFromRenderTarget(&buffer->colourTarget, false);


	Gnm::Texture depthT;
	depthT.initFromDepthRenderTarget(&buffer->depthTarget, false);


	PS4TextureNew* colorps4Text = new PS4TextureNew();
	colorps4Text->apiTexture = colorT;

	PS4TextureNew* depthps4Text = new PS4TextureNew();
	depthps4Text->apiTexture = depthT;

	PS4FrameBuffer* FrameBuffer = new PS4FrameBuffer(colorps4Text, depthps4Text);

	FrameBuffer->SetMem(colourMemory, depthMemory);

	return FrameBuffer;
}


void	PS4Renderer::DestroyMemoryAllocators()
{
	stackAllocators[GARLIC].deinit();
	stackAllocators[ONION].deinit();
}
void	PS4Renderer::DestroyVideoSystem()
{
	for (int i = 0; i < numBuffers; ++i) {
		delete FrameBuffers[i];
	}
	delete[] FrameBuffers;

	sceVideoOutClose(videoHandle);
}

void	PS4Renderer::DestroyGCMRendering()
{
	//onionAllocator->release(frames);
}



void	PS4Renderer::InitialiseGCMRendering() {
	frames = (PS4Frame*)onionAllocator.allocate(sizeof(PS4Frame) * maxCMDBuffers, alignof(PS4Frame));

	for (int i = 0; i < maxCMDBuffers; ++i) {
		new (&frames[i])PS4Frame();
	}

	sce::Gnmx::Toolkit::Allocators allocators = sce::Gnmx::Toolkit::Allocators(onionAllocator, garlicAllocator, ownerHandle);
	Gnmx::Toolkit::initializeWithAllocators(&allocators);
}


nclgl::Maths::Matrix4	PS4Renderer::GetViewMatrix()
{
	return PS4ToNcl(viewMatrix);
}

nclgl::Maths::Matrix4   PS4Renderer::GetProjMatrix()
{
	return PS4ToNcl(projMatrix);
}
void 					PS4Renderer::SetViewMatrix(nclgl::Maths::Matrix4& mat)
{
	viewMatrix = nclToPS4(mat);
}
void					PS4Renderer::SetProjMatrix(nclgl::Maths::Matrix4& mat)
{
	projMatrix = nclToPS4(mat);
}


void PS4Renderer::Resize(int x, int y)
{
	// Commented because haven't derived from the base class yet
	//width = max(x, 1);
	//height = max(y, 1);
	//SetViewPort(width, height);
}

void	PS4Renderer::SetClearColour(nclgl::Maths::Vector3& colour)
{

}

void	PS4Renderer::PrepareToRender()
{
	currentFrame->StartFrame();

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

	currentPS4FrameBuffer->SetGraphicsContext(currentGFXContext);

	SetRenderFrameBuffer(currentPS4FrameBuffer, true, true, true);

	for (auto& shader : shaders)
	{
		shader->SetGraphicsContext(currentGFXContext);
	}
	for (auto& texture : textures)
	{
		texture->SetCurrentGFXContext(currentGFXContext);
	}
	for (auto& node : nodes)
	{
		node->SetGraphicsContext(currentGFXContext);
	}

	// Put this temporarily here for now
	//Primitive Setup State
	Gnm::PrimitiveSetup primitiveSetup;
	primitiveSetup.init();
	primitiveSetup.setCullFace(Gnm::kPrimitiveSetupCullFaceNone);
	primitiveSetup.setFrontFace(Gnm::kPrimitiveSetupFrontFaceCcw);
	//primitiveSetup.setPolygonMode()
	currentGFXContext->setPrimitiveSetup(primitiveSetup);

	////Screen Access State
	Gnm::DepthStencilControl dsc;
	dsc.init();
	dsc.setDepthControl(Gnm::kDepthControlZWriteEnable, Gnm::kCompareFuncLessEqual);
	dsc.setDepthEnable(true);
	currentGFXContext->setDepthStencilControl(dsc);
}

void PS4Renderer::SwapScreenBuffer()
{
	prevScreenBuffer = currentScreenBuffer;
	currentScreenBuffer = (currentScreenBuffer + 1) % numBuffers;
	sceVideoOutSubmitFlip(videoHandle, prevScreenBuffer, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);
	currentPS4FrameBuffer = FrameBuffers[currentScreenBuffer];

}

void PS4Renderer::SwapCommandBuffer()
{
	if (currentGFXContext) {
		if (currentGFXContext->submit() != sce::Gnm::kSubmissionSuccess) {
			std::cerr << "Graphics queue submission failed?" << std::endl;
		}
		Gnm::submitDone();
	}

	currentGPUBuffer = (currentGPUBuffer + 1) % maxCMDBuffers;

	currentFrame = &frames[currentGPUBuffer];
	currentGFXContext = &currentFrame->GetCommandBuffer();
}

void	PS4Renderer::PostRender()
{
	currentFrame->EndFrame();

	framesSubmitted++;
}

void	PS4Renderer::SetRenderFrameBuffer(PS4FrameBuffer*buffer, bool clearColour, bool clearDepth, bool clearStencil) {
	currentPS4FrameBuffer = buffer;
	currentPS4FrameBuffer->SetGraphicsContext(currentGFXContext);
	currentPS4FrameBuffer->Activate();
}