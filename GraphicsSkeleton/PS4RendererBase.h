#pragma once
#include "RendererBase.h"
#include "PS4MemoryAware.h"

#include <gnm.h>
#include <gnmx\fetchshaderhelper.h>

#include <..\samples\sample_code\graphics\api_gnm\toolkit\allocators.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\stack_allocator.h>
#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>
#include "PS4Frame.h"
#include "PS4TextureNew.h"

#include "PS4FrameBuffer.h"

using namespace sce;
using namespace Gnmx;
using namespace Gnmx::Toolkit;

struct PS4ScreenBuffer  {
	sce::Gnm::RenderTarget		colourTarget;
	sce::Gnm::DepthRenderTarget depthTarget;
};

#ifdef  SHADER_OLD
class PS4Shader;
#else
class PS4ShaderNew;
#endif //  SHADER_OLD


class PS4MeshNew;

class RenderObject;

class PS4RendererBase : 
	public RendererBase, public PS4MemoryAware
{
public:
	PS4RendererBase();
	~PS4RendererBase();

	void	SwapBuffers() override;
	void	RenderScene() override;

protected:
	virtual void RenderActiveScene() = 0;

	void	SwapScreenBuffer();
	void	SwapCommandBuffer();

	void DrawRenderObject(RenderObject* o) override;

private:
	void	InitialiseMemoryAllocators();
	void	InitialiseVideoSystem();
	void	InitialiseGCMRendering();

	void	DestroyMemoryAllocators();
	void	DestroyVideoSystem();
	void	DestroyGCMRendering();

	void	SetRenderBuffer(PS4ScreenBuffer*buffer, bool clearColour, bool clearDepth, bool clearStencil);
	void SetRenderFrameBuffer(PS4FrameBuffer * buffer, bool clearColour, bool clearDepth, bool clearStencil);
	void	ClearBuffer(bool colour, bool depth, bool stencil);

	PS4ScreenBuffer* GenerateScreenBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);

	PS4FrameBuffer * GenerateScreenFrameBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);

	//void DrawMesh(PS4Mesh& mesh);

protected:
	int currentGPUBuffer;

	const int _MaxCMDBufferCount;

//VIDEO SYSTEM VARIABLES
	int videoHandle;		//Handle to video system

//SCREEN BUFFER VARIABLES
	const int			_bufferCount;	//How many screen buffers should we have
	int					currentScreenBuffer;
	int					prevScreenBuffer;
	PS4ScreenBuffer**	screenBuffers;	//Pointer to our screen buffers
	PS4FrameBuffer**	FrameBuffers;	//Pointer to our screen buffers
//Memory Allocation
	const int _GarlicMemory;
	const int _OnionMemory;

	sce::Gnmx::Toolkit::StackAllocator	stackAllocators[MEMORYMAX];

	//default data
#ifdef SHADER_OLD 
	PS4Shader*		defaultShader;
#else
	PS4ShaderNew*	defaultShader;
#endif // SHADER_OLD 

	
	PS4MeshNew*		defaultMesh;
	PS4TextureNew* defaultTexture;

	RenderObject* defaultObject;

	//Individual Frames
	PS4Frame*	frames;

	Matrix4*	viewProjMat;
	Gnm::Buffer	cameraBuffer;

	int framesSubmitted;

	//Per frame pointers...
	PS4ScreenBuffer*		currentPS4Buffer;  //Pointer to whichever buffer we're currently using...
	PS4FrameBuffer*		currentPS4FrameBuffer;
	Gnmx::GnmxGfxContext*	currentGFXContext;
	PS4Frame*				currentFrame;
};

