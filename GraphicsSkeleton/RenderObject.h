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

	RenderObject(Mesh* m, PS4ShaderNew* shader, TextureBase* tex);

	const Matrix4& GetLocalTransform() const;
	void	SetLocalTransform(const Matrix4& mat);

	PS4ShaderNew* GetShader() const {
		return shader;
	}


protected:
	Mesh*		mesh;
	PS4ShaderNew* shader;

	static const int TEXTURE_COUNT = 4;
	TextureBase* textures[TEXTURE_COUNT];

	Matrix4		localTransform;
};

