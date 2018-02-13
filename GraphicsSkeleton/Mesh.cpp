#include "Mesh.h"


Mesh::Mesh()
{
	vertices	= NULL;
	texCoords	= NULL;
	normals		= NULL;
	tangents	= NULL;
	indices		= NULL;
}

Mesh::~Mesh()
{
	delete[] vertices;
	delete[] texCoords;
	delete[] normals;
	delete[] tangents;
	delete[] indices;
}
