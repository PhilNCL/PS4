#include "ExampleRenderer.h"
#include "RenderObject.h"

ExampleRenderer::ExampleRenderer()
{
	rotation = 0.0f;
	defaultObject[0] = new RenderObject((Mesh*)defaultMesh, (ShaderBase*)defaultShader, (TextureBase*)defaultTexture);
	defaultObject[1] = new RenderObject((Mesh*)defaultMesh, (ShaderBase*)defaultShader, (TextureBase*)defaultTexture);

}

ExampleRenderer::~ExampleRenderer()
{
	delete defaultObject[0];
	delete defaultObject[1];
}

void ExampleRenderer::UpdateScene(float dt)	{
	rotation += dt;
	Matrix4::rotationZ(rotation);
	defaultObject[0]->SetLocalTransform(Matrix4::translation(Vector3(-0.4, 0, 0)) * Matrix4::rotationZ(rotation));

	defaultObject[1]->SetLocalTransform(Matrix4::translation(Vector3(0.4, 0, 0)));
}

void ExampleRenderer::RenderActiveScene() {
	DrawRenderObject(defaultObject[0]);
	DrawRenderObject(defaultObject[1]);
}