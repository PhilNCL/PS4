#pragma once

#include <utility> // pair
#include <string>

#include <gnmx.h>
#include <shader\binary.h>
#include "PS4MemoryAware.h"

#include "nclPS4Interface.h"
#include "ShaderBase.h"

#include <map>
namespace nclgl
{
	namespace Maths 
	{
		class Matrix4;
	}
}

class PS4ShaderNew : public PS4MemoryAware, public ShaderBase
{
public:
	PS4ShaderNew(const std::string& vertex, const std::string& pixel);
	~PS4ShaderNew();

	inline void SetGraphicsContext(sce::Gnmx::GnmxGfxContext* cmdList) { currentGFXContext = cmdList; }
	void Activate();

	//void SetUniform(const std::string& name, int i);
	//void SetUniform(const std::string& name, float f);
	void SetUniform(const std::string& name, const nclgl::Maths::Matrix4& mat);
	//void SetUniform(const std::string& name, const Vector2& vec);
	//void SetUniform(const std::string& name, const Vector3& vec);
	//void SetUniform(const std::string& name, const Vector4& vec);
	//void SetUniform(const std::string& name, int numMats, Matrix4* mats);
	//void SetUniform(const std::string& name, int numVecs, Vector3* vecs);
	//void SetUniform(const std::string& name, int numFloats, float* floats);

protected: // Constructor helper functions

	// Generates vertex and fetch shader from a binary file
	void GenerateVertexShader(const std::string& vertex);

	// Generates pixel shader from a binary file
	void GeneratePixelShader(const std::string& pixel);

	// Loads binary file at name store in into, dataSize is the length of the file. 
	// Returns false if cannot open file
	bool LoadShaderBinary(const std::string &name, char*& into, int& dataSize);

	// Generates the default fetch shader
	void GenerateFetchShader(char* binData);

protected: // Shaders
	sce::Gnmx::VsShader* vertexShader = nullptr;
	sce::Gnmx::PsShader* pixelShader  = nullptr;
	void*				 fetchShader;

protected: // Stores uniform date for each shader stage
	sce::Gnmx::InputOffsetsCache   vertexCache;
	sce::Gnmx::InputOffsetsCache   pixelCache;

	// Returns shader stage, uniform Location  pair
	std::pair<sce::Gnm::ShaderStage, int>	GetConstantBuffer(const std::string &name);

protected: // Contains metadata about shader for e.g. mapping to uniforms
	sce::Shader::Binary::Program   vertexBinary;
	sce::Shader::Binary::Program   pixelBinary;

protected: 
	// Current graphics context
	sce::Gnmx::GnmxGfxContext*	   currentGFXContext = nullptr;


protected:
	//// Map between each uniform and shader stage
	////TODO: Generate dynamically
	//static std::map<std::string, sce::Gnm::ShaderStage> uniformMap;
};

