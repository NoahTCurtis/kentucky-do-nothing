#include <string>
#include <iostream>

#include "glm/gtc/matrix_transform.hpp"

#include "skeleton.h"
#include "render.h"

Skeleton::Skeleton()
{}

void Skeleton::Initialize(const aiScene* scene)
{
	mRootBone = new Bone(scene->mRootNode, nullptr);

	for (int i = 0; i < (int)scene->mNumAnimations; i++)
	{
		mAnimations.push_back(scene->mAnimations[i]);
	}

	if(scene->mNumAnimations > 0)
		StartAnimation(0);
}

Skeleton::~Skeleton()
{
	//SHOULD TELL ROOT BONE TO DESTROY ITS CHILDREN
}

void Skeleton::DebugDraw()
{
	//compute animTime
	aiAnimation* anim = mAnimations[mCurrentAnimation];
	mAnimTime = mAnimTime01 * (float)anim->mDuration;
	///mAnimTime = (float)((double)mAnimTime01 * anim->mDuration / anim->mTicksPerSecond);

	//draw
	if (mRootBone != nullptr)
		mRootBone->DebugDraw();

	//std::cout << mCurrentAnimation << "================================================================" << std::endl;
}

void Skeleton::StartAnimation(int index)
{
	mCurrentAnimation = index;

	//inform the bones of their new aiNodeAnim's
	aiAnimation* anim = mAnimations[mCurrentAnimation];
	for (unsigned i = 0; i < anim->mNumChannels; i++)
	{
		aiNodeAnim* node = anim->mChannels[i];
		Bone* bone = mBoneMap[std::string(node->mNodeName.C_Str())];
		bone->mAiNodeAnim = node;
	}
}

Bone::Bone(const aiNode* node, Bone* parent)
{
	//copy data
	std::memcpy(&mTransform, &node->mTransformation, 16 * sizeof(float));
	mTransform = glm::transpose(mTransform);
	mParent = parent;
	mName = std::string(node->mName.C_Str());

	//connect self to skeleton
	mSkeleton = &(Renderer::get()->skeleTemp); //(TEMP)
	mSkeleton->mBoneMap[mName] = this;

	//calculate local data
	mCompoundTransform = (parent != nullptr) ? parent->mCompoundTransform : glm::mat4(1);
	mCompoundTransform = mCompoundTransform * mTransform;
	mAnimTransform = glm::mat4(1);

	//copy children
	mNumChildren = node->mNumChildren;
	mChildren.reserve(mNumChildren);
	for (unsigned i = 0; i < mNumChildren; i++)
	{
		mChildren.push_back(new Bone(node->mChildren[i], this));
	}
}

Bone::~Bone()
{
	//SHOULD DESTROY ALL CHILDREN
}

void Bone::DebugDraw()
{
	//extract animation data
	ComputeAnimationVQS();

	//compute worldspace start/end points
	glm::mat4 parentTransform = glm::mat4(1);
	if (mParent != nullptr)
		parentTransform = mParent->mCompoundTransform;
	glm::vec3 startpos = glm::vec3(parentTransform * glm::vec4(0, 0, 0, 1.0f));
	glm::vec3 endpos = glm::vec3(mCompoundTransform * glm::vec4(0, 0, 0, 1.0f));
	
	//create debug line
	DebugLine dl;
	dl.start = startpos;
	dl.end = endpos;
	dl.startcolor = glm::vec3(1, 0, 1);
	dl.endcolor = glm::vec3(0, 1, 0);

	//(TEMP) BADLY SCALE DOWN! REMOVE THIS!
	dl.start = dl.start * 0.02f;
	dl.end = dl.end * 0.02f;

	//skip thhis one if its not a "real bone"
	if(mName.find("IK") == std::string::npos)
		Renderer::get()->add_debug_line(dl);

	///std::cout << mName << ": (" << dl.start.x << ", " << dl.start.y << ", " << dl.start.z << ") -> ("
	///	<< dl.end.x << ", " << dl.end.y << ", " << dl.end.z << ")\n";

	for (unsigned i = 0; i < mNumChildren; i++)
		mChildren[i]->DebugDraw();
}

void Bone::ComputeAnimationVQS()
{
	if (mAiNodeAnim == nullptr) return;

	//make the keys
	aiVectorKey aiVecA;
	aiVectorKey aiVecB;
	aiQuatKey aiQuatA;
	aiQuatKey aiQuatB;
	aiVectorKey aiScaleA;
	aiVectorKey aiScaleB;

	//get the keys
	unsigned i;
	for (i = 0; i < mAiNodeAnim->mNumPositionKeys - 1; i++)
	{
		aiVecA = mAiNodeAnim->mPositionKeys[i];
		aiVecB = mAiNodeAnim->mPositionKeys[i + 1];
		if (aiVecB.mTime > mSkeleton->mAnimTime)
			break;
	}
	for(i = 0; i < mAiNodeAnim->mNumRotationKeys - 1; i++)
	{
		aiQuatA = mAiNodeAnim->mRotationKeys[i];
		aiQuatB = mAiNodeAnim->mRotationKeys[i + 1];
		if (aiQuatB.mTime > mSkeleton->mAnimTime)
			break;
	}
	for (i = 0; i < mAiNodeAnim->mNumScalingKeys - 1; i++)
	{
		aiScaleA = mAiNodeAnim->mScalingKeys[i];
		aiScaleB = mAiNodeAnim->mScalingKeys[i + 1];
		if (aiScaleB.mTime > mSkeleton->mAnimTime)
			break;
	}

	//Interpolate between A-B keys
	aiVectorKey aiVec = aiVecA;
	aiQuatKey aiQuat = aiQuatA;
	aiVectorKey aiScale = aiScale;

	glm::vec3 translateKey(aiVec.mValue.x, aiVec.mValue.y, aiVec.mValue.z);
	kdn::quat rotateKey(aiQuat.mValue);
	glm::vec3 scaleKey(aiScale.mValue.x, aiScale.mValue.y, aiScale.mValue.z);

	//create VQS matrix
	glm::mat4 ident(1);
	mAnimTransform = glm::translate(ident, translateKey) * rotateKey.mat4() * glm::scale(ident, scaleKey);
}
