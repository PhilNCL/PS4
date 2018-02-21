#include "PS4RendererBase.h"
#include <video_out.h>	//Video System


#include "PS4ShaderNew.h"


#include "PS4MeshNew.h"
#include "RenderObject.h"
#include <gnmx\basegfxcontext.h>

#include <iostream>


//Test
PS4RendererBase::PS4RendererBase()
	:
	_MaxCMDBufferCount(3),
	  _bufferCount(3),
	  _GarlicMemory(1024 * 1024 * 512),
	  _OnionMemory( 1024 * 1024 * 256)
{
	framesSubmitted		 = 0;
	currentGPUBuffer	 = 0;
	currentScreenBuffer	 = 0;
	prevScreenBuffer	 = 0;

	std::cerr << "Starting Rich Code!" << std::endl;

	currentGFXContext	 = nullptr;

	InitialiseMemoryAllocators();

	InitialiseGCMRendering();
	InitialiseVideoSystem();

	defaultShader = new PS4ShaderNew(
		"/app0/VertexShader.sb",
		"/app0/PixelShader.sb"
	);

	defaultMesh		= PS4MeshNew::GenerateQuad();
	defaultTexture = new PS4TextureNew("/app0/doge.gnf");

	viewProjMat		= (Matrix4*)onionAllocator.allocate(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	*viewProjMat	= Matrix4::identity();

	cameraBuffer.initAsConstantBuffer(viewProjMat, sizeof(Matrix4));
	cameraBuffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO); // it's a constant buffer, so read-only is OK

	SwapBuffers();
}

PS4RendererBase::~PS4RendererBase()
{
	delete defaultObject;
	delete defaultMesh;
	delete defaultTexture;
	delete defaultShader;

	DestroyGCMRendering();
	DestroyVideoSystem();
	DestroyMemoryAllocators();
}

void	PS4RendererBase::InitialiseVideoSystem() {
#ifdef OLD_BUFFERS
	screenBuffers = new PS4ScreenBuffer*[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		screenBuffers[i] = GenerateScreenBuffer(1920, 1080);
	}

	//Now we can open up the video port
	videoHandle		= sceVideoOutOpen(0, SCE_VIDEO_OUT_BUS_TYPE_MAIN, 0, NULL);

	SceVideoOutBufferAttribute attribute;
	sceVideoOutSetBufferAttribute(&attribute,
		SCE_VIDEO_OUT_PIXEL_FORMAT_B8_G8_R8_A8_SRGB,
		SCE_VIDEO_OUT_TILING_MODE_TILE,
		SCE_VIDEO_OUT_ASPECT_RATIO_16_9,
		screenBuffers[0]->colourTarget.getWidth(),
		screenBuffers[0]->colourTarget.getHeight(),
		screenBuffers[0]->colourTarget.getPitch()
	);

	void* bufferAddresses[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		bufferAddresses[i] = screenBuffers[i]->colourTarget.getBaseAddress();
	}

	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, _bufferCount, &attribute);

#else
	FrameBuffers = new PS4FrameBuffer*[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
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

	void* bufferAddresses[_bufferCount];

	for (int i = 0; i < _bufferCount; ++i) {
		bufferAddresses[i] = FrameBuffers[i]->colourTarget.at(0).getBaseAddress();
	}

	sceVideoOutRegisterBuffers(videoHandle, 0, bufferAddresses, _bufferCount, &attribute);
#endif //old buffer
}

void	PS4RendererBase::InitialiseGCMRendering() {
	frames = (PS4Frame*)onionAllocator.allocate(sizeof(PS4Frame) * _MaxCMDBufferCount, alignof(PS4Frame));

	for (int i = 0; i < _MaxCMDBufferCount; ++i) {
		new (&frames[i])PS4Frame();
	}

	sce::Gnmx::Toolkit::Allocators allocators = sce::Gnmx::Toolkit::Allocators(onionAllocator, garlicAllocator, ownerHandle);
	Gnmx::Toolkit::initializeWithAllocators(&allocators);
}

void	PS4RendererBase::InitialiseMemoryAllocators() {
	stackAllocators[GARLIC].init(SCE_KERNEL_WC_GARLIC, _GarlicMemory);
	stackAllocators[ONION ].init(SCE_KERNEL_WB_ONION , _OnionMemory);

	this->garlicAllocator   = Gnmx::Toolkit::GetInterface(&stackAllocators[GARLIC]);
	this->onionAllocator	= Gnmx::Toolkit::GetInterface(&stackAllocators[ONION]);
	Gnm::registerOwner(&ownerHandle, "PS4RendererBase");
}

void PS4RendererBase::DestroyMemoryAllocators() {
	stackAllocators[GARLIC].deinit();
	stackAllocators[ONION ].deinit();
}

PS4ScreenBuffer*	PS4RendererBase::GenerateScreenBuffer(uint width, uint height, bool colour, bool depth, bool stencil) {

	PS4ScreenBuffer* buffer = new PS4ScreenBuffer();

	if (colour) {	
		Gnm::RenderTargetSpec spec;
		spec.init();
		spec.m_width		= width;
		spec.m_height		= height;
		spec.m_numSamples	= Gnm::kNumSamples1;
		spec.m_numFragments = Gnm::kNumFragments1;
		spec.m_colorFormat	= Gnm::kDataFormatB8G8R8A8UnormSrgb;	

		GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec.m_colorTileModeHint, GpuAddress::kSurfaceTypeColorTargetDisplayable, spec.m_colorFormat, 1);

		int32_t success = buffer->colourTarget.init(&spec);

		if (success != SCE_GNM_OK) {
			bool a = true;
		}

		const Gnm::SizeAlign colourAlign = buffer->colourTarget.getColorSizeAlign();

		void *colourMemory = stackAllocators[GARLIC].allocate(colourAlign);

		Gnm::registerResource(nullptr, ownerHandle, colourMemory, colourAlign.m_size,
			"Colour", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);

		buffer->colourTarget.setAddresses(colourMemory, NULL, NULL);
	}

	if (depth) {
		Gnm::DepthRenderTargetSpec spec;
		spec.init();
		spec.m_width			= width;
		spec.m_height			= height;
		spec.m_numFragments		= Gnm::kNumFragments1;
		spec.m_zFormat			= Gnm::ZFormat::kZFormat32Float;
		spec.m_stencilFormat	= (stencil ? Gnm::kStencil8 : Gnm::kStencilInvalid);

		GpuAddress::computeSurfaceTileMode(Gnm::GpuMode::kGpuModeBase, &spec.m_tileModeHint, GpuAddress::kSurfaceTypeDepthTarget, Gnm::DataFormat::build(spec.m_zFormat), 1);

		void* stencilMemory = 0;

		int32_t success = buffer->depthTarget.init(&spec);

		if (success != SCE_GNM_OK) {
			bool a = true;
		}

		void *depthMemory = stackAllocators[GARLIC].allocate(buffer->depthTarget.getZSizeAlign());

		Gnm::registerResource(nullptr, ownerHandle, depthMemory, buffer->depthTarget.getZSizeAlign().m_size,
			"Depth", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);


		if (stencil) {
			stencilMemory = stackAllocators[GARLIC].allocate(buffer->depthTarget.getStencilSizeAlign());

			Gnm::registerResource(nullptr, ownerHandle, stencilMemory, buffer->depthTarget.getStencilSizeAlign().m_size,
				"Stencil", Gnm::kResourceTypeDepthRenderTargetBaseAddress, 0);
		}

		buffer->depthTarget.setAddresses(depthMemory, stencilMemory);
	}

	return buffer;


}



PS4FrameBuffer*	PS4RendererBase::GenerateScreenFrameBuffer(uint width, uint height, bool colour, bool depth, bool stencil) {

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



void	PS4RendererBase::DestroyGCMRendering() {
	//onionAllocator->release(frames);
}

void	PS4RendererBase::DestroyVideoSystem() {
	for (int i = 0; i < _bufferCount; ++i) {
		delete screenBuffers[i];
	}
	delete[] screenBuffers;

	sceVideoOutClose(videoHandle);
}

void PS4RendererBase::RenderScene()			{
	currentFrame->StartFrame();	

	currentGFXContext->waitUntilSafeForRendering(videoHandle, currentGPUBuffer);

#ifdef OLD_BUFFERS
	SetRenderBuffer(currentPS4Buffer, true, true, true);
#else
	SetRenderFrameBuffer(currentPS4FrameBuffer, true, true, true);
#endif //old buffers



	defaultShader->SetGraphicsContext(currentGFXContext);
	defaultShader->Activate();
	
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

	//Gnm::Sampler trilinearSampler;
	//trilinearSampler.init();
	//trilinearSampler.setMipFilterMode(Gnm::kMipFilterModeLinear);

	defaultTexture->SetCurrentGFXContext(currentGFXContext);
	defaultTexture->Bind();
	//currentGFXContext->setTextures(Gnm::kShaderStagePs, 0, 1, &defaultTexture->GetAPITexture());
	//currentGFXContext->setSamplers(Gnm::kShaderStagePs, 0, 1, &trilinearSampler);

	*viewProjMat = Matrix4::identity();

	RenderActiveScene();

	currentFrame->EndFrame();

	framesSubmitted++;
}

void PS4RendererBase::SwapBuffers()			{
	SwapScreenBuffer();
	SwapCommandBuffer();
}

void	PS4RendererBase::SwapScreenBuffer() {

	prevScreenBuffer	= currentScreenBuffer;
	currentScreenBuffer = (currentScreenBuffer + 1) % _bufferCount;
	sceVideoOutSubmitFlip(videoHandle, prevScreenBuffer, SCE_VIDEO_OUT_FLIP_MODE_VSYNC, 0);

#ifdef OLD_BUFFERS
	currentPS4Buffer = screenBuffers[currentScreenBuffer];
#else
	currentPS4FrameBuffer = FrameBuffers[currentScreenBuffer];

#endif //old buffers

}

int k; 
void	PS4RendererBase::SwapCommandBuffer() {
	if (currentGFXContext) {	
		if (currentGFXContext->submit() != sce::Gnm::kSubmissionSuccess) {
			std::cerr << "Graphics queue submission failed?" << std::endl;
		}
		Gnm::submitDone();
	}

	currentGPUBuffer	= (currentGPUBuffer + 1) % _MaxCMDBufferCount;
	 
	currentFrame		= &frames[currentGPUBuffer]; 
	currentGFXContext	= &currentFrame->GetCommandBuffer();

	std::cout << k << "\n";
	++k;


}
 
void	PS4RendererBase::SetRenderBuffer(PS4ScreenBuffer*buffer, bool clearColour, bool clearDepth, bool clearStencil) {
	currentPS4Buffer = buffer;
	currentGFXContext->setRenderTargetMask(0xF);
	currentGFXContext->setRenderTarget(0, &currentPS4Buffer->colourTarget);
	currentGFXContext->setDepthRenderTarget(&currentPS4Buffer->depthTarget);

	currentGFXContext->setupScreenViewport(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), 0.5f, 0.5f);
	currentGFXContext->setScreenScissor(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight());
	currentGFXContext->setWindowScissor(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);
	currentGFXContext->setGenericScissor(0, 0, currentPS4Buffer->colourTarget.getWidth(), currentPS4Buffer->colourTarget.getHeight(), sce::Gnm::WindowOffsetMode::kWindowOffsetDisable);

	ClearBuffer(clearColour, clearDepth, clearStencil);
}


void	PS4RendererBase::SetRenderFrameBuffer(PS4FrameBuffer*buffer, bool clearColour, bool clearDepth, bool clearStencil) {
	currentPS4FrameBuffer = buffer;
	currentPS4FrameBuffer->SetGraphicsContext(currentGFXContext);
	currentPS4FrameBuffer->Activate();
}

void	PS4RendererBase::ClearBuffer(bool colour, bool depth, bool stencil) {
	if (colour) {
		//Vector4 defaultClearColour(rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, rand() / (float)RAND_MAX, 1.0f);
		Vector4 defaultClearColour(0.1f, 0.1f, 0.1f, 1.0f);
		SurfaceUtil::clearRenderTarget(*currentGFXContext, &currentPS4Buffer->colourTarget, defaultClearColour);
	}

	if (depth) {
		float defaultDepth = 1.0f;
		SurfaceUtil::clearDepthTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, defaultDepth);
	}

	if (stencil && currentPS4Buffer->depthTarget.getStencilReadAddress()) {
		int defaultStencil = 0;
		SurfaceUtil::clearStencilTarget(*currentGFXContext, &currentPS4Buffer->depthTarget, defaultStencil);
	}
}

void PS4RendererBase::DrawRenderObject(RenderObject* o) {
	//Matrix4* transformMat = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(Matrix4), Gnm::kEmbeddedDataAlignment4);
	//*transformMat = nclToPS4( PS4ToNcl(o->GetLocalTransform()));

	//PS4ShaderNew* realShader = (PS4ShaderNew*)o->GetShader();

	//realShader->SetGraphicsContext(currentGFXContext);
	//realShader->SetUniform("RenderObjectData", PS4ToNcl(o->GetLocalTransform()));
	//realShader->SetUniform("CameraData", PS4ToNcl(Matrix4::identity()));

	//realShader->Activate();


	//defaultMesh->SetGraphicsContext(currentGFXContext);
	//defaultMesh->Draw();
	//DrawMesh(*defaultMesh);
}


//void PS4RendererBase::DrawMesh(PS4Mesh& mesh) {
//	defaultMesh->SubmitDraw(*currentGFXContext, Gnm::ShaderStage::kShaderStageVs);
//}
