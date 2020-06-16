#ifndef INCLUDED_RENDERER
#define INCLUDED_RENDERER

#pragma once

#include <Graphics/Camera.h>
#include <Graphics/Shader.h>

#include <Core/Entity.h>
#include <Core/Renderable.h>
#include <Core/Transform.h>
<<<<<<< HEAD
#include <IO/ModelImporter.h>
#include <Graphics/Framebuffer.h>
=======

#include <IO/ModelImporter.h>

>>>>>>> 5302a5fef9735646a13a986f336553b17cd0cab5
class Renderer
{
	Shader::Ptr defaultShader;
	Shader::Ptr skyboxShader;
	std::map<std::string, Shader::Ptr> shaders;

	GL::UniformBuffer<Camera::UniformData> cameraUBO;
	
	Mesh::Ptr unitCube;
	TextureCubeMap::Ptr cubeMap;
<<<<<<< HEAD
	TextureCubeMap::Ptr cubeMapHdr;
	TextureCubeMap::Ptr irradianceMap;
	TextureCubeMap::Ptr specularMap;
	Texture2D::Ptr brdfLUT;
	std::string panoFileNew;
	std::string panoFileOld;
	std::vector<Entity::Ptr> rootEntitis;
	std::vector<Entity::Ptr> entities;
	unsigned int modelIndex = 0;
	glm::vec3 position = glm::vec3(0);
	std::vector<glm::mat4> VP;
	glm::mat4 P;
	Shader::Ptr pano2cmShader;
	Shader::Ptr irradianceShader;
	Shader::Ptr specularShader;
	unsigned int maxMipLevel;
	std::vector<unsigned int> mipWidthVector;
	std::vector<unsigned int> mipHeightVector;
	std::vector<float> roughnessVector;
	Framebuffer::Ptr specFBO;
	Framebuffer::Ptr envFBO;
	


public:
	struct ReturnMaps {
		TextureCubeMap::Ptr ldrCubeMaps; TextureCubeMap::Ptr irrMaps; TextureCubeMap::Ptr specMaps; TextureCubeMap::Ptr hdrCubeMaps;
	};
=======
	TextureCubeMap::Ptr irradianceMap;
	TextureCubeMap::Ptr specularMap;
	Texture2D::Ptr brdfLUT;
	
	std::vector<Entity::Ptr> rootEntitis;
	std::vector<Entity::Ptr> entities;
	unsigned int modelIndex = 0;

public:
>>>>>>> 5302a5fef9735646a13a986f336553b17cd0cab5
	Renderer(unsigned int width, unsigned int height);
	bool init(std::string modelFile, std::string panoFile);
	void initShader();
	void initEnvMaps(std::string panoFile);
	void nextModel();
	void updateAnimations(float dt);
	void updateCamera(Camera& camera);
	void render();
<<<<<<< HEAD
	void loopVideo(Texture2D::Ptr activeHdr);
	void loopVideoAndHdr(Texture2D::Ptr activeVideoFrame, Texture2D::Ptr activeHdr);
	void loopVideoAndLdr(Texture2D::Ptr activeVideoFrame, Texture2D::Ptr activeHdr);
	ReturnMaps initCubeMaps(Texture2D::Ptr activeVideoFrame, Texture2D::Ptr activeHdr);
	void displayVideo(TextureCubeMap::Ptr ldrCubeMaps);
	void displayLighting(TextureCubeMap::Ptr irrMaps, TextureCubeMap::Ptr specMaps);
	ReturnMaps initEnvMapsDemo(std::string panoFile);
	ReturnMaps initEnvMapsDemoHdr(std::string panoFile);
	void renderNew(TextureCubeMap::Ptr ldrCubeMaps, TextureCubeMap::Ptr irrMaps, TextureCubeMap::Ptr specMaps );
=======
>>>>>>> 5302a5fef9735646a13a986f336553b17cd0cab5
};

#endif // INCLUDED_RENDERER