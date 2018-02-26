#pragma once

class PS4RenderNode;
class PS4ShaderNew;
class PS4MeshNew;
class PS4TextureNew;

#include "PS4Renderer.h"

class MiniGraphicsPipeline
{
public:
	MiniGraphicsPipeline();
	~MiniGraphicsPipeline();

	void UpdateScene(float dt);
	void RenderScene();
protected:
	PS4Renderer* renderer;
	float rotation;
	PS4RenderNode* defaultObject[2];

	PS4ShaderNew*	defaultShader;
	PS4MeshNew*		defaultMesh;
	PS4TextureNew* defaultTexture;


};

