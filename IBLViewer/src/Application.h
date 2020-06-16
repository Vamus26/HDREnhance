#ifndef INCLUDED_APPLICATION
#define INCLUDED_APPLICATION

#pragma once

#include <Graphics/Camera.h>
#include <Graphics/Renderer.h>
#include <Platform/GLWindow.h>
#include <Platform/InputHandler.h>
#include <IO/ModelImporter.h>
class Application
{
	Camera camera;
	GLWindow window;
	InputHandler input;
	Renderer renderer;
	Texture2D::Ptr pano;
	Texture2D::Ptr pano2;
	std::vector<std::string> frameNames;
	std::vector<std::string> frameNamesHdr;
	std::vector<Texture2D::Ptr> cubemaps;
	std::vector<Texture2D::Ptr> hdrMaps;
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;
	std::vector<TextureCubeMap::Ptr> filledCubeMaps;
	std::vector<TextureCubeMap::Ptr> filledIrradianceMap;
	std::vector<TextureCubeMap::Ptr> filledSpecularMap;
	std::vector<TextureCubeMap::Ptr> filledHdrCubeMaps;
public:
	Application(const char* title, unsigned int width, unsigned int height);
	bool init(std::string modelFile, std::string txtFile, std::string panoFile);//, std::string panoFileNew);
	void setupInput();
	void loop();
	//void loadNextHDR(std::string modelFile, std::string panoFile);

};

#endif // INCLUDED_APPLICATION