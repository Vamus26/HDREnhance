#include "Application.h"
#include <IO/GLTFImporter.h>
#include <IO/ImageLoader.h>
#include <IO/ShaderLoader.h>
#include <iostream>
#include <fstream>
#include <string>
#include <Graphics/Renderer.h>
using namespace std::placeholders;

Application::Application(const char* title, unsigned int width, unsigned int height) :
	window(title, width, height),
	renderer(width, height)
{
	camera.setAspect((float)width / (float)height);
}

bool Application::init(std::string modelFile,std::string videoFrames, std::string hdrFrames)//,  std::string panoFileNew)
{
	if (!window.isInitialized())
		return false;
	window.attachInput(input);

	//load frames from txtfile
	std::string line;
	std::ifstream myfile(videoFrames);
	if (myfile.is_open())
	{
		while (getline(myfile, line))
		{
			frameNames.push_back(line);
		//	cubemaps.push_back(IO::loadTextureHDR(line));
		
		}
		myfile.close();
	}
	else std::cout << "Unable to open file 1";
	std::ifstream myfileHDR(hdrFrames);
	if (myfileHDR.is_open())
	{
		while (getline(myfileHDR, line))
		{
			frameNamesHdr.push_back(line);
		//	hdrMaps.push_back(IO::loadTextureHDR(line));
		}
		myfileHDR.close();
	}
	else std::cout << "Unable to open file 2";

	if (!renderer.init(modelFile, frameNames[0]))
		return false;
	setupInput();
	for(int i =0;i<frameNames.size();i++) {
	Renderer::ReturnMaps tmpStruct = renderer.initEnvMapsDemo(frameNames[i]);
	filledCubeMaps.push_back(tmpStruct.ldrCubeMaps);
	//uncomment if LDR only
	/*filledIrradianceMap.push_back(tmpStruct.irrMaps);
	filledSpecularMap.push_back(tmpStruct.specMaps);*/

	Renderer::ReturnMaps tmpStruct2 = renderer.initEnvMapsDemo(frameNamesHdr[i]);
	//comment if LDR video
	filledIrradianceMap.push_back(tmpStruct2.irrMaps);
	filledSpecularMap.push_back(tmpStruct2.specMaps);
	//filledHdrCubeMaps.push_back(tmpStruct2.hdrCubeMaps);

	}
	
	/*for(int i=0;i<cubemaps.size();i++) {
		
		Renderer::ReturnMaps tmpStruct= renderer.initCubeMaps(cubemaps[i], hdrMaps[i]);
		filledCubeMaps.push_back(tmpStruct.ldrCubeMaps);
		filledIrradianceMap.push_back(tmpStruct.irrMaps);
		filledSpecularMap.push_back(tmpStruct.specMaps);
	}*/
	
	return true;
}

void Application::setupInput()
{
	input.addKeyCallback(GLFW_KEY_ESCAPE, GLFW_PRESS, std::bind(&GLWindow::close, &window));
	input.addKeyCallback(GLFW_KEY_W, GLFW_PRESS, std::bind(&Camera::setDirection, &camera, Camera::Direction::FORWARD));
	input.addKeyCallback(GLFW_KEY_S, GLFW_PRESS, std::bind(&Camera::setDirection, &camera, Camera::Direction::BACK));
	input.addKeyCallback(GLFW_KEY_A, GLFW_PRESS, std::bind(&Camera::setDirection, &camera, Camera::Direction::LEFT));
	input.addKeyCallback(GLFW_KEY_D, GLFW_PRESS, std::bind(&Camera::setDirection, &camera, Camera::Direction::RIGHT));
	input.addKeyCallback(GLFW_KEY_W, GLFW_RELEASE, std::bind(&Camera::releaseDirection, &camera, Camera::Direction::FORWARD));
	input.addKeyCallback(GLFW_KEY_S, GLFW_RELEASE, std::bind(&Camera::releaseDirection, &camera, Camera::Direction::BACK));
	input.addKeyCallback(GLFW_KEY_A, GLFW_RELEASE, std::bind(&Camera::releaseDirection, &camera, Camera::Direction::LEFT));
	input.addKeyCallback(GLFW_KEY_D, GLFW_RELEASE, std::bind(&Camera::releaseDirection, &camera, Camera::Direction::RIGHT));
	input.setMouseCallback(std::bind(&Camera::updateRotation, &camera, _1, _2));
}

void Application::loop()
{
	double dt = 0.0;
	double animTime = 0.0f;
	double tickTime = 1.0f / 30.0f;
	double updateTime = 0.0f;
	int frames = 0;
	int frameCounter = -1;

	while (!window.shouldClose())
	{
		double startTime = glfwGetTime();

		glfwPollEvents();

		if (animTime > tickTime)
		{
			camera.move(animTime);
			camera.rotate(animTime);
			renderer.updateCamera(camera);

			renderer.updateAnimations(startTime);
			animTime = 0.0f;
			if (frameCounter >= (filledCubeMaps.size()-1))
			{
				frameCounter = -1;
			}
		//	renderer.displayVideo(filledCubeMaps[frameCounter]);
			//renderer.displayLighting(filledIrradianceMap[frameCounter], filledSpecularMap[frameCounter]);
			//renderer.loopVideoAndLdr(cubemaps[frameCounter], hdrMaps[frameCounter]);
	//		renderer.loopVideoAndHdr(cubemaps[frameCounter], hdrMaps[frameCounter]);
		//	renderer.loopVideo(cubemaps[frameCounter]);
		//	renderer.loopHdrs(cubemaps[frameCounter]);
			frameCounter++;
			
		}

		//camera.move(dt);
		//camera.rotate(dt);

		//renderer.updateCamera(camera);
		//renderer.updateAnimations(startTime);
		
		if (frameCounter == -1)
			frameCounter++;
		renderer.renderNew(filledCubeMaps[frameCounter], filledIrradianceMap[frameCounter], filledSpecularMap[frameCounter]);
		
		
		
		window.swapBuffers();

		dt = glfwGetTime() - startTime;
		animTime += dt;
		updateTime += dt;

		if (updateTime >= 1.0)
		{
			std::string title = "PhotonRenderer, FPS: " + std::to_string(frames);
			window.setWindowTitle(title);
			updateTime = 0.0;
			frames = 0;
			
		}
		else
		{
			frames++;
		}
	}
}