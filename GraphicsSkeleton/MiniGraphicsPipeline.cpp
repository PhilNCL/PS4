#include "MiniGraphicsPipeline.h"

#include "PS4RenderNode.h"
#include "PS4ShaderNew.h"
#include "PS4TextureNew.h"

MiniGraphicsPipeline::MiniGraphicsPipeline()
{
	renderer = new PS4Renderer();

	rotation = 0.0f;

	defaultMesh = PS4MeshNew::GenerateQuad();
	defaultObject[0] = new PS4RenderNode((MeshBase*)defaultMesh);
	defaultObject[1] = new PS4RenderNode((MeshBase*)defaultMesh);

	renderer->RegisterNode(defaultObject[0]);
	renderer->RegisterNode(defaultObject[1]);

	defaultShader = new PS4ShaderNew(
		"/app0/VertexShader.sb",
		"/app0/PixelShader.sb"
	);
	renderer->RegisterShader(defaultShader);

	defaultTexture = new PS4TextureNew("/app0/doge.gnf");
	renderer->RegisterTexture(defaultTexture);

}


MiniGraphicsPipeline::~MiniGraphicsPipeline()
{
	delete defaultObject[0];
	delete defaultObject[1];

	delete renderer;
	delete defaultMesh;
	delete defaultShader;
	delete defaultTexture;
}


void MiniGraphicsPipeline::UpdateScene(float dt)
{
	rotation += dt;
	nclgl::Maths::Matrix4 rotationZ = nclgl::Maths::Matrix4::Rotation(rotation * (180 / PI), nclgl::Maths::Vector3(0.0f, 0.0f, 1.0f));

	defaultObject[0]->SetTransform(nclgl::Maths::Matrix4::Translation(nclgl::Maths::Vector3(-0.4, 0.0, 0)) * rotationZ);
	defaultObject[1]->SetTransform(nclgl::Maths::Matrix4::Translation(nclgl::Maths::Vector3(0.4, 0, 0)));
}

void MiniGraphicsPipeline::RenderScene()
{
	renderer->PrepareToRender();

	defaultShader->SetUniform("CameraData", PS4ToNcl(Matrix4::identity()));

	defaultShader->Activate();
	defaultTexture->Bind();

	for (int i = 0; i < 2; ++i)
	{
		defaultShader->SetUniform("RenderObjectData", defaultObject[i]->GetTransform());
		defaultObject[i]->Draw();
	}
	renderer->PostRender();
	renderer->SwapBuffers();
}

