#include "Animator.h"

void Animator::setSkin(BoneNode& node)
{
	for (auto anim : animations)
		anim->setBoneTree(node);
}

void Animator::addAnimation(Animation::Ptr animation)
{
	animations.push_back(animation);
}

void Animator::update(float dt)
{
	if (currentAnimation < animations.size())
		animations[currentAnimation]->update(dt);
}

void Animator::switchAnimation(unsigned int index)
{
	// TODO: implement this proberly...
	if (index < animations.size())
		currentAnimation = index;
}

bool Animator::hasRiggedAnim()
{
	return (!animations.empty());
}

std::vector<glm::mat4> Animator::getBoneTransform()
{
	std::vector<glm::mat4> boneTransforms;
	if (currentAnimation < animations.size())
		boneTransforms = animations[currentAnimation]->getBoneTransform();
	return boneTransforms;
}

std::vector<glm::mat3> Animator::getNormalTransform()
{
	std::vector<glm::mat3> normalTransforms;
	if (currentAnimation < animations.size())
		normalTransforms = animations[currentAnimation]->getNormalTransform();
	return normalTransforms;
}