#pragma once

#include "./vectormath/scalar_cpp/vectormath_aos.h"

using namespace sce::Vectormath::Scalar::Aos;
class Mesh
{
public:
	Mesh();
	virtual ~Mesh();

protected:
	unsigned int numVertices;
	unsigned int numIndices;

	Vector3* vertices;
	Vector2* texCoords;
	Vector3* normals;
	Vector3* tangents;

	int*	 indices;
};

