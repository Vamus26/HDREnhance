#ifndef INCLUDED_ANIMATOR
#define INCLUDED_ANIMATOR

#pragma once

#include "Component.h"
#include "Transform.h"

#include <Graphics/Animation.h>

class Animator : public Component
{
private:
	std::vector<Animation::Ptr> animations;
	unsigned int currentAnimation = 0;
	
public:
	Animator() {}
	void setSkin(BoneNode& node);
	void addAnimation(Animation::Ptr animation);
	void update(float dt);
	void switchAnimation(unsigned int index);
	bool hasRiggedAnim();
	std::vector<glm::mat4> getBoneTransform();
	std::vector<glm::mat3> getNormalTransform();

	typedef std::shared_ptr<Animator> Ptr;
	static Ptr create()
	{
		return std::make_shared<Animator>();
	}

};

#endif // INCLUDED_ANIMATOR