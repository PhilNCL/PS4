#include "RenderObject.h"

RenderObject::RenderObject()
{
	mesh	= nullptr;
	shader	= nullptr;

	for (int i = 0; i < TEXTURE_COUNT; ++i) {
		textures[i] = nullptr;
	}
}

RenderObject::~RenderObject()
{

}

RenderObject::RenderObject(Mesh* m, ShaderBase* s, TextureBase* t) {
	mesh		= m;
	shader		= s;
	textures[0] = t;
}

const Matrix4& RenderObject::GetLocalTransform() const {
	return localTransform;
}

void	RenderObject::SetLocalTransform(const Matrix4& mat) {
	localTransform = mat;
}