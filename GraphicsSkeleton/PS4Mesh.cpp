#include "PS4Mesh.h"

PS4Mesh::PS4Mesh()
{
	indexBuffer		= 0;
	vertexBuffer	= 0;
	attributeCount	= 0;
}


PS4Mesh::~PS4Mesh()
{
	delete[] attributeBuffers;
}

PS4Mesh* PS4Mesh::GenerateQuad() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->numVertices	= 4;
	mesh->numIndices	= 4;
	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriStrip;

	mesh->vertices  = new Vector3[mesh->numVertices];
	mesh->texCoords = new Vector2[mesh->numVertices];
	mesh->normals   = new Vector3[mesh->numVertices];
	mesh->tangents  = new Vector3[mesh->numVertices];
	mesh->indices   = new int[mesh->numIndices];

	//mesh->vertices[0] = Vector3(-1.0f, -1.0f, 0.0f);
	//mesh->vertices[1] = Vector3(-1.0f,  1.0f, 0.0f);
	//mesh->vertices[2] = Vector3( 1.0f, -1.0f, 0.0f);
	//mesh->vertices[3] = Vector3( 1.0f,  1.0f, 0.0f);

	mesh->texCoords[0] = Vector2(0.0f, 1.0f);
	mesh->texCoords[1] = Vector2(0.0f, 0.0f);
	mesh->texCoords[2] = Vector2(1.0f, 1.0f);
	mesh->texCoords[3] = Vector2(1.0f, 0.0f);

	mesh->vertices[0] = Vector3(-0.5f, -0.5f, 0.0f);
	mesh->vertices[1] = Vector3(+0.5f, -0.5f, 0.0f);
	mesh->vertices[2] = Vector3(-0.5f, +0.5f, 0.0f);
	mesh->vertices[3] = Vector3(+0.5f, +0.5f, 0.0f);


	for (int i = 0; i < mesh->numVertices; ++i) {
		mesh->normals[i]  = Vector3(0, 0, 1);
		mesh->tangents[i] = Vector3(1, 0, 0);
		mesh->indices[i]  = i;
	}

	mesh->BufferData();
	return mesh;
}

PS4Mesh* PS4Mesh::GenerateSinglePoint() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->numVertices	= 1;
	mesh->numIndices	= 1;
	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypePointList;

	mesh->vertices	= new Vector3[mesh->numVertices];
	mesh->texCoords = new Vector2[mesh->numVertices];
	mesh->normals	= new Vector3[mesh->numVertices];
	mesh->tangents	= new Vector3[mesh->numVertices];
	mesh->indices	= new int[mesh->numIndices];

	mesh->vertices[0] = Vector3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < mesh->numVertices; ++i) {
		mesh->normals[i]	= Vector3(0, 0, 1);
		mesh->tangents[i]	= Vector3(1, 0, 0);
		mesh->indices[i]	= i;
	}

	mesh->BufferData();
	return mesh;
}

PS4Mesh* PS4Mesh::GenerateTriangle() {
	PS4Mesh* mesh = new PS4Mesh();

	mesh->numVertices	= 3;
	mesh->numIndices	= 3;
	mesh->indexType		= sce::Gnm::IndexSize::kIndexSize32;
	mesh->primitiveType = sce::Gnm::PrimitiveType::kPrimitiveTypeTriList;

	mesh->vertices = new Vector3[mesh->numVertices];
	mesh->texCoords = new Vector2[mesh->numVertices];
	mesh->normals = new Vector3[mesh->numVertices];
	mesh->tangents = new Vector3[mesh->numVertices];
	mesh->indices = new int[mesh->numIndices];

	mesh->vertices[0] = Vector3(0.0f, 0.5f, 0.0f); 
	mesh->vertices[1] = Vector3(0.5f, -0.5f, 0.0f);
	mesh->vertices[2] = Vector3(-0.5f, -0.5f, 0.0f);

	mesh->texCoords[0] = Vector2(0.5f, 0.0f);
	mesh->texCoords[1] = Vector2(1.0f, 1.0f);
	mesh->texCoords[2] = Vector2(0.0f, 1.0f);

	for (int i = 0; i < mesh->numVertices; ++i) {
		mesh->normals[i] = Vector3(0, 0, 1);
		mesh->tangents[i] = Vector3(1, 0, 0);
		mesh->indices[i] = i;
	}

	mesh->BufferData();
	return mesh;
}

void	PS4Mesh::BufferData() {
	vertexDataSize = numVertices * sizeof(MeshVertex);
	indexDataSize  = numIndices * sizeof(int);

	indexBuffer = static_cast<int*>			(garlicAllocator.allocate(indexDataSize, Gnm::kAlignmentOfBufferInBytes));
	vertexBuffer = static_cast<MeshVertex*>	(garlicAllocator.allocate(vertexDataSize, Gnm::kAlignmentOfBufferInBytes));

	Gnm::registerResource(nullptr, ownerHandle, indexBuffer , indexDataSize , "IndexData" , Gnm::kResourceTypeIndexBufferBaseAddress, 0);
	Gnm::registerResource(nullptr, ownerHandle, vertexBuffer, vertexDataSize, "VertexData", Gnm::kResourceTypeIndexBufferBaseAddress, 0);

	for (int i = 0; i < numVertices; ++i) {
		memcpy(&vertexBuffer[i].position,	  &vertices[i],	 sizeof(float) * 3);
		memcpy(&vertexBuffer[i].textureCoord, &texCoords[i], sizeof(float) * 2);
		memcpy(&vertexBuffer[i].normal,		  &normals[i],   sizeof(float) * 3);
		memcpy(&vertexBuffer[i].tangent,	  &tangents[i],  sizeof(float) * 3);
	}

	for (int i = 0; i < numIndices; ++i) { //Our index buffer might not have the same data size as the source indices?
		indexBuffer[i] = indices[i];
	}	

	attributeCount		= 4;
	attributeBuffers	= new sce::Gnm::Buffer[4];

	InitAttributeBuffer(attributeBuffers[0], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].position));
	InitAttributeBuffer(attributeBuffers[1], Gnm::kDataFormatR32G32Float	, &(vertexBuffer[0].textureCoord));
	InitAttributeBuffer(attributeBuffers[2], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].normal));
	InitAttributeBuffer(attributeBuffers[3], Gnm::kDataFormatR32G32B32Float, &(vertexBuffer[0].tangent));
}

void	PS4Mesh::InitAttributeBuffer(sce::Gnm::Buffer &buffer, Gnm::DataFormat format, void*offset) {
	buffer.initAsVertexBuffer(offset, format, sizeof(MeshVertex), numVertices);
	buffer.setResourceMemoryType(Gnm::kResourceMemoryTypeRO);
}

void PS4Mesh::SubmitDraw(Gnmx::GnmxGfxContext& cmdList, Gnm::ShaderStage stage) {
	cmdList.setVertexBuffers(stage, 0, attributeCount, attributeBuffers);
	cmdList.setPrimitiveType(primitiveType);
	cmdList.setIndexSize(indexType);
	cmdList.drawIndex(numIndices, indexBuffer);
} 