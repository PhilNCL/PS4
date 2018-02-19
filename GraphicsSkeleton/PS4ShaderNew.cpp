#include "PS4ShaderNew.h"

#include <gnmx/shader_parser.h> // ShaderInfo 
#include <graphics\api_gnm\toolkit\shader_loader.h>  // calculateMemoryRequiredForVsFetchShader()

#include <iostream>
#include <fstream>

#include "nclPS4Interface.h"

using sce::Gnm::ShaderStage;

using std::string;
using std::pair;

//std::map<std::string, sce::Gnm::ShaderStage> PS4ShaderNew::uniformMap = 
//{
//	{"RenderObjectData", sce::Gnm::kShaderStageVs },
//	{ "CameraData", sce::Gnm::kShaderStageVs }
//};

// Helper function returns true if file has a .sb extension
bool IsBinary(const std::string& name)
{
	if (name.size() > 3)
	{
		return name.substr(name.size() - 3) == ".sb";
	}
	else
	{
		return false;
	}
}

PS4ShaderNew::PS4ShaderNew(const std::string& vertex, const std::string& pixel)
{
	//loadSuccess = false;

	fetchShader = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;

	if (!IsBinary(vertex) || !IsBinary(pixel))
	{
		return;
	}
	GenerateVertexShader(vertex);
	GeneratePixelShader(pixel);
	//loadSuccess = true;
}


PS4ShaderNew::~PS4ShaderNew()
{
}

void PS4ShaderNew::GeneratePixelShader(const string& pixel)
{
	char*	binData = NULL;				// resulting compiled shader bytes
	int		binSize = 0;				// Size of bin data
	sce::Gnmx::ShaderInfo shaderInfo;	// Temporary object stores the offset into binData where shader begins


	if (LoadShaderBinary(pixel, binData, binSize)) {
		pixelBinary.loadFromMemory(binData, binSize);
		sce::Gnmx::parseShader(&shaderInfo, binData);

		void* shaderBinary = garlicAllocator.allocate(shaderInfo.m_gpuShaderCodeSize, sce::Gnm::kAlignmentOfShaderInBytes);
		void* shaderHeader = onionAllocator.allocate(shaderInfo.m_psShader->computeSize(), sce::Gnm::kAlignmentOfBufferInBytes);

		memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
		memcpy(shaderHeader, shaderInfo.m_psShader, shaderInfo.m_psShader->computeSize());

		pixelShader = (sce::Gnmx::PsShader*)shaderHeader;
		pixelShader->patchShaderGpuAddress(shaderBinary);
	}
	else
	{
		std::cout << "Failed to generate pixel shader: " << pixel << " from binary. " << std::endl;
		return;
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, pixel.c_str(), sce::Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputOffsetsCache(&pixelCache, Gnm::kShaderStagePs, pixelShader);

	//delete binData;
}

void PS4ShaderNew::GenerateVertexShader(const std::string& vertex)
{
	char*	binData = NULL;				// resulting compiled shader bytes
	int		binSize = 0;				// Size of bin data
	sce::Gnmx::ShaderInfo shaderInfo;	// Temporary object stores the offset into binData where shader begins

	if (LoadShaderBinary(vertex, binData, binSize)) 
	{
			vertexBinary.loadFromMemory(binData, binSize);

			sce::Gnmx::parseShader(&shaderInfo, binData);

			void* shaderBinary = garlicAllocator.allocate(shaderInfo.m_gpuShaderCodeSize, sce::Gnm::kAlignmentOfShaderInBytes);
			void* shaderHeader = onionAllocator.allocate(shaderInfo.m_vsShader->computeSize(), sce::Gnm::kAlignmentOfBufferInBytes);

			memcpy(shaderBinary, shaderInfo.m_gpuShaderCode, shaderInfo.m_gpuShaderCodeSize);
			memcpy(shaderHeader, shaderInfo.m_vsShader, shaderInfo.m_vsShader->computeSize());

			vertexShader = (sce::Gnmx::VsShader*)shaderHeader;
			vertexShader->patchShaderGpuAddress(shaderBinary);
	}
	else 
	{
		std::cout << "Failed to generate vertex shader: " << vertex << " from binary. " << std::endl;
		return;
	}

	sce::Gnm::registerResource(nullptr, ownerHandle, vertexShader->getBaseAddress(),
		shaderInfo.m_gpuShaderCodeSize, vertex.c_str(), sce::Gnm::kResourceTypeShaderBaseAddress, 0);

	sce::Gnmx::generateInputOffsetsCache(&vertexCache, sce::Gnm::kShaderStageVs, vertexShader);

	GenerateFetchShader(binData);
	//delete binData;
}

bool PS4ShaderNew::LoadShaderBinary(const string &name, char*& into, int& dataSize) {
	std::ifstream binFile(name, std::ios::binary);

	if (!binFile) {
		return false;
	}
	binFile.seekg(0, ios::end);
	int size = binFile.tellg();

	into = new char[size];

	binFile.seekg(0, ios::beg);
	binFile.read(into, size);

	dataSize = size;

	return true;
}

//bool PS4ShaderNew::LoadShaderBinary(const string &name, char*& into, int& dataSize) {
//	std::ifstream binFile(name, std::ios::binary, std::ios::ate);
//
//	if (!binFile) {
//		return false;
//	}
//
//	dataSize = binFile.tellg();
//	into = new char[dataSize];
//
//	binFile.seekg(0, ios::beg);
//	binFile.read(into, dataSize);
//
//	return true;
//}

void PS4ShaderNew::GenerateFetchShader(char* binData) {
	fetchShader = garlicAllocator.allocate(sce::Gnmx::Toolkit::calculateMemoryRequiredForVsFetchShader(binData));

	uint32_t shaderModifier;
	sce::Gnmx::generateVsFetchShader(fetchShader, &shaderModifier, vertexShader);

	vertexShader->applyFetchShaderModifier(shaderModifier);
}


void PS4ShaderNew::Activate()
{
	if (currentGFXContext)
	{
		currentGFXContext->setActiveShaderStages(sce::Gnm::kActiveShaderStagesVsPs);
				   
		currentGFXContext->setVsShader(vertexShader, 0, fetchShader, &vertexCache);
		currentGFXContext->setPsShader(pixelShader, &pixelCache);
	}

}


pair<ShaderStage, int> PS4ShaderNew::GetConstantBuffer(const std::string &name)
{
	sce::Shader::Binary::Buffer* constantBuffer = vertexBinary.getBufferResourceByName(name.c_str());

	if (constantBuffer)
	{
		return std::make_pair<ShaderStage, int>(sce::Gnm::kShaderStageVs, (int)constantBuffer->m_resourceIndex);
	}
	else
	{
		constantBuffer = pixelBinary.getBufferResourceByName(name.c_str());
		if (!constantBuffer)
		{
			std::cout << "Warning failed to find buffer resource: " << name << std::endl;
			return std::make_pair<ShaderStage, int>(sce::Gnm::kShaderStagePs, -1);
		}
		else
		{
			return std::make_pair<ShaderStage, int>(sce::Gnm::kShaderStagePs, (int)constantBuffer->m_resourceIndex);
		}
		
	}
}



//void PS4ShaderNew::SetUniform(const std::string& name, int i)
//{
//
//}
//
//void PS4ShaderNew::SetUniform(const std::string& name, float f)
//{
//
//}
//
void PS4ShaderNew::SetUniform(const std::string& name, const nclgl::Maths::Matrix4& mat)
{
	if (currentGFXContext)
	{
		// Allocate memory for buffer matrix
		Matrix4* transformMat = (Matrix4*)currentGFXContext->allocateFromCommandBuffer(sizeof(Matrix4), sce::Gnm::kEmbeddedDataAlignment4);
		*transformMat = nclToPS4(mat);

		// Create (read-only) uniform buffer
		sce::Gnm::Buffer constantBuffer;
		constantBuffer.initAsConstantBuffer(transformMat, sizeof(Matrix4));
		constantBuffer.setResourceMemoryType(sce::Gnm::kResourceMemoryTypeRO);

		// Send uniform data to correct shader location
		pair<ShaderStage, int> uniformLocation = GetConstantBuffer(name);
		currentGFXContext->setConstantBuffers(uniformLocation.first, uniformLocation.second, 1, &constantBuffer);
	}
	else
	{
		std::cout << "Tried to set uniform without a valid context" << std::endl;
	}

}
//
//void PS4ShaderNew::SetUniform(const std::string& name, const Vector2& vec)
//{
//
//}
//
//void PS4ShaderNew::SetUniform(const std::string& name, const Vector3& vec)
//{
//
//}
//
//void PS4ShaderNew::SetUniform(const std::string& name, const Vector4& vec)
//{
//
//}
//
//void PS4ShaderNew::SetUniform(const std::string& name, int numMats, Matrix4* mats)
//{
//
//}
//
//void PS4ShaderNew::SetUniform(const std::string& name, int numVecs, Vector3* vecs)
//{
//
//}
//
//void PS4ShaderNew::SetUniform(const std::string& name, int numFloats, float* floats)
//{
//
//}
	

