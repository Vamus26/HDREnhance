#include "Renderer.h"

#include <Graphics/Framebuffer.h>
#include <Graphics/Primitives.h>
#include <Graphics/Shader.h>

#include <IO/GLTFImporter.h>
#include <IO/ImageLoader.h>
#include <IO/ShaderLoader.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h> 
#include <Windows.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

namespace json = rapidjson;

void extern debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* param)
{
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
	{
		std::cout << std::hex << std::endl;
		std::cout << "OpenGL error: type=" << type << " severity: " << severity << " message: " << message << std::endl;
	}
}

Renderer::Renderer(unsigned int width, unsigned int height)
{
}

bool Renderer::init(std::string modelFile, std::string panoFile)
{
	//glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, 1280, 720);
	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(debugCallback, 0);
	panoFileNew = panoFile;
	panoFileOld = panoFile;
	initShader();
	initEnvMaps(panoFile);

	IO::GLTFImporter importer;
    auto root = importer.importModel(modelFile);

	rootEntitis.push_back(root);
	entities = importer.getEntities();
	importer.clear();

	cameraUBO.bindBase(0);
	 
	return true;
}


void Renderer::loopVideo(Texture2D::Ptr pano) {
		pano2cmShader->use();

		glDisable(GL_DEPTH_TEST);
		{
			int size = 256;
			cubeMap = TextureCubeMap::create(size, size, GL::RGB32F);
			cubeMap->generateMipmaps();
			cubeMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);

			auto envFBO = Framebuffer::create(size, size);
			envFBO->addRenderTexture(GL::COLOR0, cubeMap);
			envFBO->checkStatus();
			envFBO->begin();
			
			pano->use(0);

			unitCube->draw();
			envFBO->end();

			cubeMap->generateMipmaps();
		}
		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, 1280, 720);	
}
void Renderer::displayVideo(TextureCubeMap::Ptr ldrCubeMap) {
	//pano2cmShader->use();

	glDisable(GL_DEPTH_TEST);
	{
		cubeMap = ldrCubeMap;
		cubeMap->use(0);
		//ldrCubeMap->use(0);
	//	unitCube->draw();
	}
//	glEnable(GL_DEPTH_TEST);
	//glViewport(0, 0, 1280, 720);
}
void Renderer::displayLighting(TextureCubeMap::Ptr irrMaps, TextureCubeMap::Ptr specMaps) {
	//pano2cmShader->use();

	glDisable(GL_DEPTH_TEST);
	{
		irradianceMap = irrMaps;
		specularMap = specMaps;
		irradianceMap->use(0);
		specularMap->use(0);
	//	irrMaps->use(0);
	//	specMaps->use(0);
	//	unitCube->draw();
	}
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 1280, 720);
}


void Renderer::loopVideoAndLdr(Texture2D::Ptr pano, Texture2D::Ptr panoHdr)
{
	

	glDisable(GL_DEPTH_TEST);
	//ldr video play
	pano2cmShader->use();
	{
		envFBO->checkStatus();
		envFBO->begin();
		pano->use(0);
		unitCube->draw();
		envFBO->end();
		cubeMap->generateMipmaps();

	}
	//end ldr video play

	irradianceShader->use();
	{
		int size = 32;
		irradianceMap = TextureCubeMap::create(size, size, GL::RGB32F);
		auto irrFBO = Framebuffer::create(size, size);
		irrFBO->addRenderTexture(GL::COLOR0, irradianceMap);
		irrFBO->checkStatus();
		irrFBO->begin();
		cubeMap->use(0);
		unitCube->draw();
		irrFBO->end();
	}

	specularShader->use();
	{
		for (unsigned int mip = 0; mip < maxMipLevel; mip++)
		{
			specularShader->setUniform("roughness", roughnessVector[mip]);
			specFBO->resize(mipWidthVector[mip], mipHeightVector[mip]);
			specFBO->addRenderTexture(GL::COLOR0, specularMap, mip);
			specFBO->begin();
			cubeMap->use(0);
			unitCube->draw();
			specFBO->end();
		}
	}
	
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 1280, 720);
}
void Renderer::loopVideoAndHdr(Texture2D::Ptr pano, Texture2D::Ptr panoHdr)
{
	pano2cmShader->use();

	glDisable(GL_DEPTH_TEST);
	{
		envFBO->checkStatus();
		envFBO->begin();
		panoHdr->use(0);
		unitCube->draw();
		envFBO->end();
	}
	//2 cubemaps(cubeMap->use(0);) anlegen/ 1xhdr 1xldr zum trennen
	irradianceShader->use();
	{
		int size = 32;
		irradianceMap = TextureCubeMap::create(size, size, GL::RGB32F);
		auto irrFBO = Framebuffer::create(size, size);
		irrFBO->addRenderTexture(GL::COLOR0, irradianceMap);
		irrFBO->checkStatus();
		irrFBO->begin();
		cubeMap->use(0);
		unitCube->draw();
		irrFBO->end();
	}

	specularShader->use();
	{
		for (unsigned int mip = 0; mip < maxMipLevel; mip++)
		{
			specularShader->setUniform("roughness", roughnessVector[mip]);
			specFBO->resize(mipWidthVector[mip], mipHeightVector[mip]);
			specFBO->addRenderTexture(GL::COLOR0, specularMap, mip);
			specFBO->begin();
			cubeMap->use(0);
			unitCube->draw();
			specFBO->end();
		}
	}
	//ldr video play
	pano2cmShader->use();
	{
		envFBO->checkStatus();
		envFBO->begin();
		pano->use(0);
		unitCube->draw();
		envFBO->end();
		cubeMap->generateMipmaps();
	}
	//end ldr video play
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 1280, 720);
}

Renderer::ReturnMaps Renderer::initCubeMaps(Texture2D::Ptr pano, Texture2D::Ptr panoHdr)
{
	pano2cmShader->use();

	glDisable(GL_DEPTH_TEST);
	{
		envFBO->checkStatus();
		envFBO->begin();
		panoHdr->use(0);
		unitCube->draw();
		envFBO->end();
	}
	//2 cubemaps(cubeMap->use(0);) anlegen/ 1xhdr 1xldr zum trennen
	irradianceShader->use();
	{
		int size = 32;
		irradianceMap = TextureCubeMap::create(size, size, GL::RGB32F);
		auto irrFBO = Framebuffer::create(size, size);
		irrFBO->addRenderTexture(GL::COLOR0, irradianceMap);
		irrFBO->checkStatus();
		irrFBO->begin();
		cubeMap->use(0);
		unitCube->draw();
		irrFBO->end();
	//	filledIrradianceMap.push_back(irradianceMap);
	}

	specularShader->use();
	{
		for (unsigned int mip = 0; mip < maxMipLevel; mip++)
		{
			specularShader->setUniform("roughness", roughnessVector[mip]);
			specFBO->resize(mipWidthVector[mip], mipHeightVector[mip]);
			specFBO->addRenderTexture(GL::COLOR0, specularMap, mip);
			specFBO->begin();
			cubeMap->use(0);
			unitCube->draw();
			specFBO->end();
		}
		//filledSpecularMap.push_back(specularMap);
	}
	//ldr video play
	pano2cmShader->use();
	{
		envFBO->checkStatus();
		envFBO->begin();
		pano->use(0);
		unitCube->draw();
		envFBO->end();
		cubeMap->generateMipmaps();
	//	filledCubeMaps.push_back(cubeMap);
	}
	//end ldr video play
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, 1280, 720);
	ReturnMaps retMap;
	retMap.ldrCubeMaps = cubeMap;
	retMap.irrMaps = irradianceMap;
	retMap.specMaps = specularMap;
	return retMap;
}

void Renderer::initShader()
{
	std::string shaderPath = "src/Shaders";
	auto shaderList = IO::loadShadersFromPath(shaderPath);
	for (auto s : shaderList)
		shaders.insert(std::pair(s->getName(), s));
	defaultShader = shaders["Default"];
	defaultShader->setUniform("material.baseColorFactor", glm::vec4(1.0f));
	defaultShader->setUniform("material.alphaCutOff", 0.0f);
	defaultShader->setUniform("material.baseColorTex", 0);
	defaultShader->setUniform("material.pbrTex", 1);
	defaultShader->setUniform("material.normalTex", 2);
	defaultShader->setUniform("material.occlusionTex", 3);
	defaultShader->setUniform("material.emissiveTex", 4);

	defaultShader->setUniform("irradianceMap", 5);
	defaultShader->setUniform("specularMap", 6);
	defaultShader->setUniform("brdfLUT", 7);

	skyboxShader = shaders["Skybox"];
	skyboxShader->setUniform("envMap", 0);
}

void Renderer::initEnvMaps(std::string panoFile)
{
	pano2cmShader = shaders["PanoToCubeMap"];
	irradianceShader = shaders["IBLDiffuseIrradiance"];
	specularShader = shaders["IBLSpecular"];
	auto integrateBRDFShader = shaders["IBLIntegrateBRDF"];

	auto pano = IO::loadTextureHDR(panoFile);

	unitCube = Primitives::createCube(glm::vec3(0), 1.0f);
	glm::vec3 position = glm::vec3(0);
	std::vector<glm::mat4> VP;

	glm::mat4 P = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	pano2cmShader->setUniform("M", glm::mat4(1.0f));
	pano2cmShader->setUniform("VP[0]", VP);
	pano2cmShader->setUniform("panorama", 0);
	pano2cmShader->use();

	glDisable(GL_DEPTH_TEST);
	{
		int size = 1024;
		cubeMap = TextureCubeMap::create(size, size, GL::RGB32F);
		cubeMap->generateMipmaps();
		cubeMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);

		/*auto */envFBO = Framebuffer::create(size, size);
		envFBO->addRenderTexture(GL::COLOR0, cubeMap);
		envFBO->checkStatus();//check ob FBO in ordnung
		envFBO->begin(); //setzt FBO als primäres renderobjekt
		pano->use(0);
		unitCube->draw();
		envFBO->end();

		cubeMap->generateMipmaps();
		//cubemap bereit
		//vector.pushback mit cubemaps + probes
		//fbo nicht speichern nur zum anlegen
		//vector.pushback(cubemap) smartpointer class
		//kein clearn nötig bei smartpointer
		//nachpushback kann man whsl die HDR frames clearn
		//offline speichern, auf festplatte und dann cubemape laden
		//benötigt lib. vllt in zukunft
	}

	irradianceShader->setUniform("VP[0]", VP);
	irradianceShader->setUniform("environmentMap", 0);
	irradianceShader->use();

	{
		int size = 32;
		irradianceMap = TextureCubeMap::create(size, size, GL::RGB32F);
		auto irrFBO = Framebuffer::create(size, size);
		irrFBO->addRenderTexture(GL::COLOR0, irradianceMap);
		irrFBO->checkStatus();
		irrFBO->begin();
		cubeMap->use(0);
		unitCube->draw();
		irrFBO->end();

	//irr MAp pushback
	}

	specularShader->setUniform("VP[0]", VP);
	specularShader->setUniform("environmentMap", 0);
	specularShader->use();

	{
		int size = 256;
		specularMap = TextureCubeMap::create(size, size, GL::RGB32F);
		specularMap->generateMipmaps();
		specularMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);

		/*auto */specFBO = Framebuffer::create(size, size);
		maxMipLevel = 8;
		
		for (unsigned int mip = 0; mip < maxMipLevel; mip++)
		{
			unsigned int mipWidth = size * std::pow(0.5, mip);
			unsigned int mipHeight = size * std::pow(0.5, mip);
			float roughness = (float)mip / (float)(maxMipLevel - 1);
			mipWidthVector.push_back(mipWidth);
			mipHeightVector.push_back(mipHeight);
			roughnessVector.push_back(roughness);
			specularShader->setUniform("roughness", roughness);

			specFBO->resize(mipWidth, mipHeight);
			specFBO->addRenderTexture(GL::COLOR0, specularMap, mip);
			specFBO->begin();
			cubeMap->use(0);
			unitCube->draw();
			specFBO->end();

		
		}
		//specMap pushback alle "8"stufen
	}

	glEnable(GL_DEPTH_TEST);
	//einmalig
	auto screenQuad = Primitives::createQuad(glm::vec3(0.0f), 2.0f);

	{
		int size = 512;
		brdfLUT = Texture2D::create(size, size, GL::RG16F);
		brdfLUT->setWrap(GL::CLAMP_TO_EDGE);

		integrateBRDFShader->use();
		auto brdfFBO = Framebuffer::create(size, size);
		brdfFBO->addRenderTexture(GL::COLOR0, brdfLUT);
		brdfFBO->begin();
		screenQuad->draw();
		brdfFBO->end();
	}

	glViewport(0, 0, 1280, 720);
}
Renderer::ReturnMaps Renderer::initCubeMapLDR(std::string panoFile)
{
	pano2cmShader = shaders["PanoToCubeMap"];
	auto pano = IO::loadTextureHDR(panoFile);

	unitCube = Primitives::createCube(glm::vec3(0), 1.0f);
	glm::vec3 position = glm::vec3(0);
	std::vector<glm::mat4> VP;

	glm::mat4 P = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	pano2cmShader->setUniform("M", glm::mat4(1.0f));
	pano2cmShader->setUniform("VP[0]", VP);
	pano2cmShader->setUniform("panorama", 0);
	pano2cmShader->use();

	glDisable(GL_DEPTH_TEST);
	{
		int size = 1024;
		cubeMap = TextureCubeMap::create(size, size, GL::RGB8);

		cubeMap->generateMipmaps();
		cubeMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);

		envFBO = Framebuffer::create(size, size);
		envFBO->addRenderTexture(GL::COLOR0, cubeMap);
		envFBO->checkStatus();//check ob FBO in ordnung
		envFBO->begin(); //setzt FBO als primäres renderobjekt
		pano->use(0);
		unitCube->draw();
		envFBO->end();
		cubeMap->generateMipmaps();
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, 1280, 720);
	ReturnMaps retMap;
	retMap.ldrCubeMaps = cubeMap;
	return retMap;
}
Renderer::ReturnMaps Renderer::initEnvMapsForLDR(std::string panoFile)
{
	pano2cmShader = shaders["PanoToCubeMap"];
	irradianceShader = shaders["IBLDiffuseIrradiance"];
	specularShader = shaders["IBLSpecular"];
	auto integrateBRDFShader = shaders["IBLIntegrateBRDF"];

	auto pano = IO::loadTextureHDR(panoFile);

	unitCube = Primitives::createCube(glm::vec3(0), 1.0f);
	glm::vec3 position = glm::vec3(0);
	std::vector<glm::mat4> VP;

	glm::mat4 P = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	pano2cmShader->setUniform("M", glm::mat4(1.0f));
	pano2cmShader->setUniform("VP[0]", VP);
	pano2cmShader->setUniform("panorama", 0);
	pano2cmShader->use();

	glDisable(GL_DEPTH_TEST);
	{
		int size = 1024;
		cubeMap = TextureCubeMap::create(size, size, GL::RGB8);//RGB8? bei LDR -RGB32F

		cubeMap->generateMipmaps();
		cubeMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);

		/*auto */envFBO = Framebuffer::create(size, size);
		envFBO->addRenderTexture(GL::COLOR0, cubeMap);
		envFBO->checkStatus();//check ob FBO in ordnung
		envFBO->begin(); //setzt FBO als primäres renderobjekt
		pano->use(0);
		unitCube->draw();
		envFBO->end();

		cubeMap->generateMipmaps();
		//cubemap bereit
		//vector.pushback mit cubemaps + probes
		//fbo nicht speichern nur zum anlegen
		//vector.pushback(cubemap) smartpointer class
		//kein clearn nötig bei smartpointer
		//nachpushback kann man whsl die HDR frames clearn
		//offline speichern, auf festplatte und dann cubemape laden
		//benötigt lib. vllt in zukunft
	}

	irradianceShader->setUniform("VP[0]", VP);
	irradianceShader->setUniform("environmentMap", 0);
	irradianceShader->use();

	{
		int size = 32;
		irradianceMap = TextureCubeMap::create(size, size, GL::RGB32F);
		auto irrFBO = Framebuffer::create(size, size);
		irrFBO->addRenderTexture(GL::COLOR0, irradianceMap);
		irrFBO->checkStatus();
		irrFBO->begin();
		cubeMap->use(0);
		unitCube->draw();
		irrFBO->end();

		//irr MAp pushback
	}

	specularShader->setUniform("VP[0]", VP);
	specularShader->setUniform("environmentMap", 0);
	specularShader->use();

	{
		int size = 256;
		specularMap = TextureCubeMap::create(size, size, GL::RGB32F);
		specularMap->generateMipmaps();
		specularMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);

		/*auto */specFBO = Framebuffer::create(size, size);
		maxMipLevel = 8;

		for (unsigned int mip = 0; mip < maxMipLevel; mip++)
		{
			unsigned int mipWidth = size * std::pow(0.5, mip);
			unsigned int mipHeight = size * std::pow(0.5, mip);
			float roughness = (float)mip / (float)(maxMipLevel - 1);
			mipWidthVector.push_back(mipWidth);
			mipHeightVector.push_back(mipHeight);
			roughnessVector.push_back(roughness);
			specularShader->setUniform("roughness", roughness);

			specFBO->resize(mipWidth, mipHeight);
			specFBO->addRenderTexture(GL::COLOR0, specularMap, mip);
			specFBO->begin();
			cubeMap->use(0);
			unitCube->draw();
			specFBO->end();


		}
		//specMap pushback alle "8"stufen
	}

	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, 1280, 720);
	ReturnMaps retMap;
	retMap.ldrCubeMaps = cubeMap;
	retMap.irrMaps = irradianceMap;
	retMap.specMaps = specularMap;
	return retMap;
}

Renderer::ReturnMaps Renderer::initEnvMapsForHDR(std::string panoFile)
{
	pano2cmShader = shaders["PanoToCubeMap"];
	irradianceShader = shaders["IBLDiffuseIrradiance"];
	specularShader = shaders["IBLSpecular"];
	auto integrateBRDFShader = shaders["IBLIntegrateBRDF"];

	auto pano = IO::loadTextureHDR(panoFile);

	unitCube = Primitives::createCube(glm::vec3(0), 1.0f);
	glm::vec3 position = glm::vec3(0);
	std::vector<glm::mat4> VP;

	glm::mat4 P = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	pano2cmShader->setUniform("M", glm::mat4(1.0f));
	pano2cmShader->setUniform("VP[0]", VP);
	pano2cmShader->setUniform("panorama", 0);
	pano2cmShader->use();

	glDisable(GL_DEPTH_TEST);
	{
		int size = 1024;
		cubeMap = TextureCubeMap::create(size, size, GL::RGB32F);//RGB8? bei LDR -RGB32F

		cubeMap->generateMipmaps();
		cubeMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);

		/*auto */envFBO = Framebuffer::create(size, size);
		envFBO->addRenderTexture(GL::COLOR0, cubeMap);
		envFBO->checkStatus();//check ob FBO in ordnung
		envFBO->begin(); //setzt FBO als primäres renderobjekt
		pano->use(0);
		unitCube->draw();
		envFBO->end();

		cubeMap->generateMipmaps();
		//cubemap bereit
		//vector.pushback mit cubemaps + probes
		//fbo nicht speichern nur zum anlegen
		//vector.pushback(cubemap) smartpointer class
		//kein clearn nötig bei smartpointer
		//nachpushback kann man whsl die HDR frames clearn
		//offline speichern, auf festplatte und dann cubemape laden
		//benötigt lib. vllt in zukunft
	}

	irradianceShader->setUniform("VP[0]", VP);
	irradianceShader->setUniform("environmentMap", 0);
	irradianceShader->use();

	{
		int size = 32;
		irradianceMap = TextureCubeMap::create(size, size, GL::RGB32F);
		auto irrFBO = Framebuffer::create(size, size);
		irrFBO->addRenderTexture(GL::COLOR0, irradianceMap);
		irrFBO->checkStatus();
		irrFBO->begin();
		cubeMap->use(0);
		unitCube->draw();
		irrFBO->end();

		//irr MAp pushback
	}

	specularShader->setUniform("VP[0]", VP);
	specularShader->setUniform("environmentMap", 0);
	specularShader->use();

	{
		int size = 256;
		specularMap = TextureCubeMap::create(size, size, GL::RGB32F);
		specularMap->generateMipmaps();
		specularMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);

		/*auto */specFBO = Framebuffer::create(size, size);
		maxMipLevel = 8;

		for (unsigned int mip = 0; mip < maxMipLevel; mip++)
		{
			unsigned int mipWidth = size * std::pow(0.5, mip);
			unsigned int mipHeight = size * std::pow(0.5, mip);
			float roughness = (float)mip / (float)(maxMipLevel - 1);
			mipWidthVector.push_back(mipWidth);
			mipHeightVector.push_back(mipHeight);
			roughnessVector.push_back(roughness);
			specularShader->setUniform("roughness", roughness);

			specFBO->resize(mipWidth, mipHeight);
			specFBO->addRenderTexture(GL::COLOR0, specularMap, mip);
			specFBO->begin();
			cubeMap->use(0);
			unitCube->draw();
			specFBO->end();


		}
		//specMap pushback alle "8"stufen
	}

	glEnable(GL_DEPTH_TEST);


	glViewport(0, 0, 1280, 720);
	ReturnMaps retMap;
	retMap.ldrCubeMaps = cubeMap;
	retMap.irrMaps = irradianceMap;
	retMap.specMaps = specularMap;
	return retMap;
}

//Renderer::ReturnMaps Renderer::initEnvMapsDemoHdr(std::string panoFile)
//{
//	pano2cmShader = shaders["PanoToCubeMap"];
//	irradianceShader = shaders["IBLDiffuseIrradiance"];
//	specularShader = shaders["IBLSpecular"];
//	auto integrateBRDFShader = shaders["IBLIntegrateBRDF"];
//
//	auto pano = IO::loadTextureHDR(panoFile);
//
//	unitCube = Primitives::createCube(glm::vec3(0), 1.0f);
//	glm::vec3 position = glm::vec3(0);
//	std::vector<glm::mat4> VP;
//
//	glm::mat4 P = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 1000.0f);
//	VP.push_back(P * glm::lookAt(position, position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
//	VP.push_back(P * glm::lookAt(position, position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
//	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
//	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
//	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
//	VP.push_back(P * glm::lookAt(position, position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
//
//	pano2cmShader->setUniform("M", glm::mat4(1.0f));
//	pano2cmShader->setUniform("VP[0]", VP);
//	pano2cmShader->setUniform("panorama", 0);
//	pano2cmShader->use();
//
//	glDisable(GL_DEPTH_TEST);
//	{
//		int size = 1024;
//		cubeMap = TextureCubeMap::create(size, size, GL::RGB32F);
//		cubeMap->generateMipmaps();
//		cubeMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);
//
//		/*auto */envFBO = Framebuffer::create(size, size);
//		envFBO->addRenderTexture(GL::COLOR0, cubeMap);
//		envFBO->checkStatus();//check ob FBO in ordnung
//		envFBO->begin(); //setzt FBO als primäres renderobjekt
//		pano->use(0);
//		unitCube->draw();
//		envFBO->end();
//
//		cubeMap->generateMipmaps();
//		//cubemap bereit
//		//vector.pushback mit cubemaps + probes
//		//fbo nicht speichern nur zum anlegen
//		//vector.pushback(cubemap) smartpointer class
//		//kein clearn nötig bei smartpointer
//		//nachpushback kann man whsl die HDR frames clearn
//		//offline speichern, auf festplatte und dann cubemape laden
//		//benötigt lib. vllt in zukunft
//	}
//
//	irradianceShader->setUniform("VP[0]", VP);
//	irradianceShader->setUniform("environmentMap", 0);
//	irradianceShader->use();
//
//	{
//		int size = 32;
//		irradianceMap = TextureCubeMap::create(size, size, GL::RGB32F);
//		auto irrFBO = Framebuffer::create(size, size);
//		irrFBO->addRenderTexture(GL::COLOR0, irradianceMap);
//		irrFBO->checkStatus();
//		irrFBO->begin();
//		cubeMap->use(0);
//		unitCube->draw();
//		irrFBO->end();
//
//		//irr MAp pushback
//	}
//
//	specularShader->setUniform("VP[0]", VP);
//	specularShader->setUniform("environmentMap", 0);
//	specularShader->use();
//
//	{
//		int size = 256;
//		specularMap = TextureCubeMap::create(size, size, GL::RGB32F);
//		specularMap->generateMipmaps();
//		specularMap->setFilter(GL::LINEAR_MIPMAP_LINEAR);
//
//		/*auto */specFBO = Framebuffer::create(size, size);
//		maxMipLevel = 8;
//
//		for (unsigned int mip = 0; mip < maxMipLevel; mip++)
//		{
//			unsigned int mipWidth = size * std::pow(0.5, mip);
//			unsigned int mipHeight = size * std::pow(0.5, mip);
//			float roughness = (float)mip / (float)(maxMipLevel - 1);
//			mipWidthVector.push_back(mipWidth);
//			mipHeightVector.push_back(mipHeight);
//			roughnessVector.push_back(roughness);
//			specularShader->setUniform("roughness", roughness);
//
//			specFBO->resize(mipWidth, mipHeight);
//			specFBO->addRenderTexture(GL::COLOR0, specularMap, mip);
//			specFBO->begin();
//			cubeMap->use(0);
//			unitCube->draw();
//			specFBO->end();
//
//
//		}
//		//specMap pushback alle "8"stufen
//	}
//
//	glEnable(GL_DEPTH_TEST);
//	//einmalig
//	auto screenQuad = Primitives::createQuad(glm::vec3(0.0f), 2.0f);
//
//	{
//		int size = 512;
//		brdfLUT = Texture2D::create(size, size, GL::RG16F);
//		brdfLUT->setWrap(GL::CLAMP_TO_EDGE);
//
//		integrateBRDFShader->use();
//		auto brdfFBO = Framebuffer::create(size, size);
//		brdfFBO->addRenderTexture(GL::COLOR0, brdfLUT);
//		brdfFBO->begin();
//		screenQuad->draw();
//		brdfFBO->end();
//	}
//
//	glViewport(0, 0, 1280, 720);
//	ReturnMaps retMap;
//	retMap.ldrCubeMaps = cubeMap;
//	retMap.irrMaps = irradianceMap;
//	retMap.specMaps = specularMap;
//	return retMap;
//}

void Renderer::updateAnimations(float dt)
{
	//std::cout << "update animation" << std::endl;
	// apply transformations from animations
	auto rootEntity = rootEntitis[modelIndex];
	auto animators = rootEntity->getChildrenWithComponent<Animator>();
 	for (auto e : animators)
	{
		auto a = e->getComponent<Animator>();
		auto t = e->getComponent<Transform>();

		a->update(dt);

		//defaultProgram.setUniform("hasAnimations", false);

		if (a->hasRiggedAnim())
		{
			auto boneTransforms = a->getBoneTransform();
			auto normalTransforms = a->getNormalTransform();
			defaultShader->setUniform("bones[0]", boneTransforms);
			defaultShader->setUniform("normals[0]", normalTransforms);
			defaultShader->setUniform("hasAnimations", true);
		}
	}

	// propagate the transformations trough the scene hirarchy
	rootEntity->getComponent<Transform>()->update(glm::mat4(1.0f));
}

void Renderer::updateCamera(Camera& camera)
{
	Camera::UniformData cameraData;
	camera.writeUniformData(cameraData);
	cameraUBO.upload(&cameraData, 1);
}

void Renderer::nextModel()
{
	modelIndex = (++modelIndex) % (unsigned int)rootEntitis.size();
}

void Renderer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	defaultShader->use();

	irradianceMap->use(5);
	specularMap->use(6);
	brdfLUT->use(7);

	auto e = rootEntitis[modelIndex];
	{
		// TODO: do depth/tansparent sorting
		auto models = e->getChildrenWithComponent<Renderable>();
		std::vector<Entity*> transparentEntities;
		std::vector<Entity*> opaqueEntities;
		for (auto m : models)
		{
			auto r = m->getComponent<Renderable>();
			if (r->useBlending())
				transparentEntities.push_back(m);
			else
				opaqueEntities.push_back(m);
		}
		for (auto m : opaqueEntities)
		{
			auto r = m->getComponent<Renderable>();
			auto t = m->getComponent<Transform>();

			t->setUniforms(defaultShader);
			r->render(defaultShader);
		}
		for (auto m : transparentEntities)
		{
			auto r = m->getComponent<Renderable>();
			auto t = m->getComponent<Transform>();

			t->setUniforms(defaultShader);
			r->render(defaultShader);
		}
	}

	skyboxShader->use();
	cubeMap->use(0);
	unitCube->draw();
}

void Renderer::renderOnlyLDR(TextureCubeMap::Ptr ldrCubeMaps)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	defaultShader->use();

	irradianceMap->use(5);
	specularMap->use(6);
	brdfLUT->use(7);

	auto e = rootEntitis[modelIndex];
	{
		// TODO: do depth/tansparent sorting
		auto models = e->getChildrenWithComponent<Renderable>();
		std::vector<Entity*> transparentEntities;
		std::vector<Entity*> opaqueEntities;
		for (auto m : models)
		{
			auto r = m->getComponent<Renderable>();
			if (r->useBlending())
				transparentEntities.push_back(m);
			else
				opaqueEntities.push_back(m);
		}
		for (auto m : opaqueEntities)
		{
			auto r = m->getComponent<Renderable>();
			auto t = m->getComponent<Transform>();

			t->setUniforms(defaultShader);
			r->render(defaultShader);
		}
		for (auto m : transparentEntities)
		{
			auto r = m->getComponent<Renderable>();
			auto t = m->getComponent<Transform>();

			t->setUniforms(defaultShader);
			r->render(defaultShader);
		}
	}

	skyboxShader->use();
	cubeMap = ldrCubeMaps;
	cubeMap->use(0);
	unitCube->draw();
}

void Renderer::renderNew(TextureCubeMap::Ptr ldrCubeMaps, TextureCubeMap::Ptr irrMaps, TextureCubeMap::Ptr specMaps)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	defaultShader->use();
	irradianceMap = irrMaps;
	irradianceMap->use(5);
	specularMap = specMaps;
	specularMap->use(6);
	brdfLUT->use(7);

	auto e = rootEntitis[modelIndex];
	{
		// TODO: do depth/tansparent sorting
		auto models = e->getChildrenWithComponent<Renderable>();
		std::vector<Entity*> transparentEntities;
		std::vector<Entity*> opaqueEntities;
		for (auto m : models)
		{
			auto r = m->getComponent<Renderable>();
			if (r->useBlending())
				transparentEntities.push_back(m);
			else
				opaqueEntities.push_back(m);
		}
		for (auto m : opaqueEntities)
		{
			auto r = m->getComponent<Renderable>();
			auto t = m->getComponent<Transform>();

			t->setUniforms(defaultShader);
			r->render(defaultShader);
		}
		for (auto m : transparentEntities)
		{
			auto r = m->getComponent<Renderable>();
			auto t = m->getComponent<Transform>();

			t->setUniforms(defaultShader);
			r->render(defaultShader);
		}
	}

	skyboxShader->use();
	cubeMap = ldrCubeMaps;
	cubeMap->use(0);
	unitCube->draw();
}