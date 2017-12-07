#pragma once

#include <fstream>
#include <iostream>
class ShaderLoader
{
public:
	ShaderLoader();
	~ShaderLoader();

	unsigned int Load(const char * filePath);
private:

};

ShaderLoader::ShaderLoader()
{
}

ShaderLoader::~ShaderLoader()
{
}

inline unsigned int ShaderLoader::Load(const char * filePath)
{
	char* shaderText;
	std::ifstream shaderFile;
	shaderFile.open(filePath);
	shaderFile >> shaderText;
	shaderFile.close();
}
