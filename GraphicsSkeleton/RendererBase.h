#pragma once

class RenderObject;

class RendererBase
{
public:
	RendererBase();
	virtual ~RendererBase();

	virtual void UpdateScene(float dt)	= 0;
	virtual void RenderScene()			= 0;
	virtual void SwapBuffers()			= 0;

protected:
	virtual void DrawRenderObject(RenderObject* o) = 0;
};

