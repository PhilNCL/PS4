#pragma once

class ShaderBase;
class Mesh;
class TextureBase;

#include <..\samples\sample_code\graphics\api_gnm\toolkit\toolkit.h>
#include "PS4ShaderNew.h"

class RenderObject
{
public:
	RenderObject();

	~RenderObject();

	RenderObject(Mesh* m, ShaderBase* shader, TextureBase* tex);

	const Matrix4& GetLocalTransform() const;
	void	SetLocalTransform(const Matrix4& mat);

	ShaderBase* GetShader() const {
		return shader;
	}


protected:
	Mesh*		mesh;
	ShaderBase* shader;

	static const int TEXTURE_COUNT = 4;
	TextureBase* textures[TEXTURE_COUNT];

	Matrix4		localTransform;
};

