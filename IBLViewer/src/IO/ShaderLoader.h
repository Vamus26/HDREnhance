#ifndef INCLUDED_SHADERLOADER
#define INCLUDED_SHADERLOADER

#pragma once

#include <Graphics/Shader.h>

namespace IO
{
#ifndef _WIN32
	AAssetManager* const asset_manager_;
#endif // !1

	
	//ArrayList<std::string> items;
	std::string loadTxtFile(const std::string& fileName);
	std::string loadExpanded(const std::string& fileName);
	std::vector<std::string> getAllFileNames(const std::string& path, const std::string& extension = "");
	std::vector<Shader::Ptr> loadShadersFromPath(const std::string& path);
	//std::string loadShaderCode(AAssetManager* assetManager, const std::string& fileName);
}

#endif // INCLUDED_SHADERLOADER