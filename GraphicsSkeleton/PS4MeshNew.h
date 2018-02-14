#pragma once

#include "PS4MemoryAware.h"
#include "Mesh.h"

class TextureBase;

namespace sce
{
	namespace Vectormath
	{
		namespace Scalar
		{
			namespace Aos
			{
				class Vector2;
				class Vector3;
			}
		}
	}
}


class PS4MeshNew : public PS4MemoryAware, public Mesh
{
public:
	PS4MeshNew();
	~PS4MeshNew();

	void Draw();
	void SetTexture(TextureBase* texture);

	inline void SetGraphicsContext(sce::Gnmx::GnmxGfxContext* context) { currentGfxContext = context; }

	static PS4MeshNew* GenerateQuad();

protected: // GPU data
	void	BufferData();

	// Will store vertex data as an interleaved buffer on the GPU 
	struct MeshVertex
	{
		float position[3];
		float textureCoord[2];
		float normal[3];
		float tangent[3];
	};

	int*		indexBuffer;
	MeshVertex*	vertexBuffer;

	// Vertex Attributes
	int					numAttributes;
	sce::Gnm::Buffer*	attributeBuffers;

protected: // Buffer data funtions
	void AllocateBuffers();
	void CopyDataToBuffers();
	void SetupVertexAttribs();

	// Marks buffer as read only and marks buffer as containing vertex information
	void InitAttributeBuffer(sce::Gnm::Buffer &buffer, sce::Gnm::DataFormat format, void*offset);

protected: // GenerateShape Functions
	void AllocateVertexMemory();

	//Quad Functions
	void AssignQuadProperties();
	void DefineQuadVertices();
	void DefineQuadTexCoords();
	void SetupQuadVectors();

protected: // Vertex Data
	unsigned int numVertices = 0;
	unsigned int numIndices = 0;

	sce::Vectormath::Scalar::Aos::Vector3* vertices;
	sce::Vectormath::Scalar::Aos::Vector2* texCoords;
	sce::Vectormath::Scalar::Aos::Vector3* normals;
	sce::Vectormath::Scalar::Aos::Vector3* tangents;

	int*	 indices;


protected: // 
	sce::Gnmx::GnmxGfxContext* currentGfxContext = nullptr;


protected:
	// Representation of indices e.g. kIndexSize32
	sce::Gnm::IndexSize		indexType;
	// Primitive type e.g. lines, trianglestrip
	sce::Gnm::PrimitiveType primitiveType;
};