#pragma once
#include <vector>
#include <string>

using std::vector;
using std::string;

class ShaderBase
{
public:
	ShaderBase();
	~ShaderBase();

protected:
	vector<string> filenames;
};

