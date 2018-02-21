#pragma once
#include "PS4RendererBase.h"
class ExampleRenderer :
	public PS4RendererBase
{
public:
	ExampleRenderer();
	~ExampleRenderer();

	void UpdateScene(float dt)	override;

protected:

	void RenderActiveScene() override;


	float rotation;
	PS4RenderNode* defaultObject[2];
};

