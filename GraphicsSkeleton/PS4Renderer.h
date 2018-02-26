#pragma once

#include <gnmx\context.h>
#include <gnm.h>

#include "PS4MemoryAware.h"
#include <graphics\api_gnm\toolkit\geommath\geommath.h>

#include <vector>

class PS4FrameBuffer;
class PS4Frame;
class PS4RenderNode;

namespace Renderer
{
	enum Clear { COLOUR, DEPTH, COLOUR_DEPTH };
	enum Culling { FRONT, BACK };
};

namespace nclgl
{
	namespace Maths
	{
		class Matrix4;
		class Vector3;
	}
}

//class Vector3;
class PS4ScreenBuffer;

class PS4ShaderNew;
class PS4TextureNew;

class PS4Renderer : public PS4MemoryAware
{
	
public:
	PS4Renderer();
	~PS4Renderer();

	// Make override when integrate
	void	SwapBuffers();

	void	SetViewPort(int width, int height);
	void	Clear(Renderer::Clear clearType);
	void	SetClearColour(nclgl::Maths::Vector3& colour);

	// PREPARE TO RENDER PREPARE TO RENDER PREPARE TO RENDER PREPARE TO RENDER
	void	PrepareToRender();
	void	PostRender();
	void	BindScreenFramebuffer();

	void Resize(int x, int y);

	void	SetScreenCulling(Renderer::Culling type);
	void	SetDefaultSettings();

	// Getters/Setters
	nclgl::Maths::Matrix4	GetViewMatrix();
	nclgl::Maths::Matrix4   GetProjMatrix();
	void 					SetViewMatrix(nclgl::Maths::Matrix4& mat);
	void					SetProjMatrix(nclgl::Maths::Matrix4& mat);

	void					RegisterShader(PS4ShaderNew* s) { shaders.push_back(s); }
	void					RegisterTexture(PS4TextureNew* s) { textures.push_back(s); }
	void					RegisterNode(PS4RenderNode* s) { nodes.push_back(s); }

protected:// Setup functions 
	void			 InitialiseMemoryAllocators();
	void			 InitialiseVideoSystem();
	void			 InitialiseGCMRendering();
	PS4ScreenBuffer* GenerateScreenBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);
	PS4FrameBuffer*	 GenerateScreenFrameBuffer(uint width, uint height, bool colour = true, bool depth = true, bool stencil = false);

protected: // Clean up functions
	void			DestroyMemoryAllocators();
	void			DestroyVideoSystem();
	void			DestroyGCMRendering();
protected: // States
	sce::Gnm::PrimitiveSetup	  primitiveSetup;
	sce::Gnm::BlendControl		  blendControlSetup;
	sce::Gnm::DepthStencilControl depthControlSetup;

	sce::Gnmx::GnmxGfxContext*	  currentGFXContext;
	sce::Gnm::RenderTarget*		  colourTarget;
	sce::Gnm::DepthRenderTarget*  depthTarget;

protected: // Swap buffer
	void SwapScreenBuffer();
	void SwapCommandBuffer();
	//SCREEN BUFFER VARIABLES
	const int			numBuffers;			//How many screen buffers should we have
	int					currentScreenBuffer;
	int					prevScreenBuffer;
	//PS4ScreenBuffer**	screenBuffers;	//Pointer to our screen buffers
	PS4FrameBuffer**	FrameBuffers;	//Pointer to our screen buffers

										//Per frame pointers...
	PS4ScreenBuffer*		currentPS4Buffer;  //Pointer to whichever buffer we're currently using...
	PS4FrameBuffer*			currentPS4FrameBuffer;

	// COMMAND BUFFER VARIABLES (i.e. bits I don't understand very well)
	const int   maxCMDBuffers;
	PS4Frame*	currentFrame;
	PS4Frame*	frames;
	int currentGPUBuffer = 0;
	int framesSubmitted;
	void	SetRenderFrameBuffer(PS4FrameBuffer * buffer, bool clearColour, bool clearDepth, bool clearStencil);

	//VIDEO SYSTEM VARIABLES
	int videoHandle;		//Handle to video system

	// Memory Buses
	sce::Gnmx::Toolkit::StackAllocator	stackAllocators[MEMORYMAX];
	const int garlicMemoryBytes;
	const int onionMemoryBytes;

	sce::Vectormath::Scalar::Aos::Matrix4 projMatrix;
	sce::Vectormath::Scalar::Aos::Matrix4 viewMatrix;

	std::vector<PS4TextureNew*> textures;
	std::vector<PS4RenderNode*> nodes;
	std::vector<PS4ShaderNew*> shaders;
};