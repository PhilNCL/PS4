#include "ExampleRenderer.h"
#include "RenderObject.h"

#include "PS4RenderNode.h"

ExampleRenderer::ExampleRenderer()
{
	rotation = 0.0f;
	defaultObject[0] = new PS4RenderNode((MeshBase*)defaultMesh);
	defaultObject[1] = new PS4RenderNode((MeshBase*)defaultMesh);

}

ExampleRenderer::~ExampleRenderer()
{
	delete defaultObject[0];
	delete defaultObject[1];
}

void ExampleRenderer::UpdateScene(float dt)	{
	rotation += dt;
	nclgl::Maths::Matrix4 rotationZ = nclgl::Maths::Matrix4::Rotation(rotation * (180 / PI), nclgl::Maths::Vector3(0.0f, 0.0f, 1.0f));
	
	defaultObject[0]->SetTransform(nclgl::Maths::Matrix4::Translation(nclgl::Maths::Vector3(-0.4, 0.0, 0)) * rotationZ);
	defaultObject[1]->SetTransform(nclgl::Maths::Matrix4::Translation(nclgl::Maths::Vector3(0.4, 0, 0)));
}

void ExampleRenderer::RenderActiveScene() 
{
	defaultShader->SetGraphicsContext(currentGFXContext);
	
	defaultShader->SetUniform("CameraData", PS4ToNcl(Matrix4::identity()));

	defaultShader->Activate();

	for (int i = 0; i < 2; ++i)
	{
		defaultObject[i]->SetGraphicsContext(currentGFXContext);
		auto x = defaultObject[i]->GetTransform();
		defaultShader->SetUniform("RenderObjectData", defaultObject[i]->GetTransform());

		defaultObject[i]->Draw();
	}

}