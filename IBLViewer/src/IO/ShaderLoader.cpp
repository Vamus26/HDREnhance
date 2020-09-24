#include "ShaderLoader.h"

#include <filesystem>
#include <fstream>
#include <sstream>
#ifndef _WIN32
	#include <C:\Users\LukasNew\AppData\Local\Android\Sdk\ndk\21.0.6113669\toolchains\llvm\prebuilt\windows-x86_64\sysroot\usr\include\android\asset_manager.h>
#endif 
namespace fs = std::filesystem;

namespace IO
{
	std::string loadTxtFile(const std::string& fileName)
	{
		std::ifstream file(fileName);
		std::stringstream ss;

		if (file.is_open())
		{
			ss << file.rdbuf();
		}
		else
		{
			std::cout << "could not open file " << fileName << std::endl;
		}

		return ss.str();
	}

	std::string loadExpanded(const std::string& fileName)
	{
		std::string code = loadTxtFile(fileName);
		std::stringstream is(code);
		std::string line;
		std::string expandedCode = "";
		while (std::getline(is, line))
		{
			if (!line.empty() && line.at(0) == '#')
			{
				size_t index = line.find_first_of(" ");
				std::string directive = line.substr(0, index);
				if (directive.compare("#include") == 0)
				{
					size_t start = line.find_first_of("\"") + 1;
					size_t end = line.find_last_of("\"");
					size_t index = fileName.find_last_of("/");
					std::string includeFile = fileName.substr(0, index) + "/" + line.substr(start, end - start);
					std::string includeCode = loadTxtFile(includeFile);
					expandedCode += includeCode;
				}
				else
				{
					expandedCode += line + "\n";
				}
			}
			else
			{
				expandedCode += line + "\n";
			}
		}

		expandedCode += '\0';

		return expandedCode;
	}

	std::vector<std::string> getAllFileNames(const std::string& path, const std::string& extension)
	{
	#ifdef _WIN32

		if (!fs::exists(path))
			std::cout << "path " << path << " does not exist!" << std::endl;

		if (!fs::is_directory(path))
			std::cout << "path " << path << " is not a directory!" << std::endl;

		std::vector<std::string> fileNames;
		for (auto& file : fs::directory_iterator(path))
		{
			if (fs::is_regular_file(file))
			{
				std::string fileName = file.path().filename().string();
				if (extension.empty())
				{
					fileNames.push_back(fileName);
				}
				else
				{
					std::string ext = file.path().extension().string();
					if (ext == extension)
					{
						fileNames.push_back(fileName);
					}
				}
			}
		}
		return fileNames;
	#endif //WIN32
	/*
		AAssetDir* assetDir = AAssetManager_openDir(asset_manager_, path.c_str());//""
		const char* filename = (const char*)NULL;
		while ((filename = AAssetDir_getNextFileName(assetDir)) != NULL) 
		{
			AAsset* asset = AAssetManager_open(asset_manager_, filename, AASSET_MODE_STREAMING);
			char buf[BUFSIZ];
			int nb_read = 0;
			FILE* out = fopen(filename, "w");
			while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0)
				fwrite(buf, nb_read, 1, out);
			fclose(out);
			AAsset_close(asset);
		}*/
	/*	AAssetDir_close(assetDir);
		AAssetManager* assetManager = getApplicationContext().getAssets();
		for (std::string file : assetManager.list("")) {
			if (file.endsWith(".txt"))
				items.add(file);
		}*/

	//	loadShaderCode(asset_manager_, path);
		std::cout << "This is not Windows." << std::endl;

	}
	/*
	std::string loadShaderCode(AAssetManager* assetManager, std::string fileName)
	{
		AAsset* asset = AAssetManager_open(assetManager, fileName.c_str(), AASSET_MODE_BUFFER);
		unsigned int len = AAsset_getLength(asset);
		std::vector<char> buffer(len + 1, '\0');
		AAsset_read(asset, buffer.data(), len);
		AAsset_close(asset);
		return std::string(buffer.data()); //return inhalt vom shader (nur text)
	}
	*/
	std::vector<Shader::Ptr> loadShadersFromPath(const std::string& path)
	{
		auto filenames = getAllFileNames(path); // check subdirectories...
		std::vector<Shader::Ptr> shaderList;
		std::map<std::string, std::vector<std::string>> shaderFiles;
		for (auto fn : filenames)
		{
			int index = fn.find_last_of('.');
			std::string name = fn.substr(0, index);
			if (shaderFiles.find(name) == shaderFiles.end())
				shaderFiles[name] = std::vector<std::string>();
			shaderFiles[name].push_back(fn);
		}
		for (auto shaderName : shaderFiles)
		{
			auto name = shaderName.first;
			auto stageList = shaderName.second;
			if (stageList.size() > 1)
			{
				bool success = true;
				auto shader = Shader::create(name);
				for (auto shaderFile : shaderName.second)
				{
					int index = shaderFile.find_last_of('.') + 1;
					int len = shaderFile.length() - index;

					std::string stage = shaderFile.substr(index, len);
					if (stage.compare("vert") == 0)
						success = shader->compile<GL::VertexShader>(loadExpanded(path + "/" + shaderFile));
					else if (stage.compare("geom") == 0)
						success = shader->compile<GL::GeometryShader>(loadExpanded(path + "/" + shaderFile));
					else if (stage.compare("frag") == 0)
						success = shader->compile<GL::FragmentShader>(loadExpanded(path + "/" + shaderFile));

					if (!success)
						break;
				}

				if (success)
				{
					shader->link();
					shaderList.push_back(shader);
				}
				else
				{
					std::cout << "error loading shader " << name << std::endl;
				}
			}
		}
		return shaderList;
	}
}
