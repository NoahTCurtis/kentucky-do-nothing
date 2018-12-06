#include <string>
#include <iostream>
#include <stack>

#include "glm/gtc/matrix_transform.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/vector_angle.hpp"

#include "skeleton.h"
#include "render.h"
#include "input.h"
#include "kdn_math.h"
#include "globals.h"
#include "mesh.h"

Skeleton::Skeleton()
{
	ik = new IK(*this);
}

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
	delete mRootBone;
	delete ik;
}

void Skeleton::Update(float dt)
{
	//position the animation
	glm::vec3 oldPos = modelToWorld.v;
	float scalef = 0.01f;
	modelToWorld.s = glm::vec3(scalef);
	if (Globals.animateOnCurve)
		modelToWorld.v = Globals.curve(mCurveTime01);
	else
		modelToWorld.v = glm::vec3(0);
	Globals.computedMoveSpeed = glm::mix(Globals.computedMoveSpeed, glm::length(oldPos - modelToWorld.v), 0.5); //movespeed checker

	//rotate the mesh and
	//draw the tangent from the mesh
	if (Globals.animateOnCurve)
	{
		DebugLine tanLine;
		tanLine.start = Renderer::get()->skeleTemp.modelToWorld.v;
		tanLine.end = Renderer::get()->skeleTemp.modelToWorld.v
			+ Globals.curve.tangent(Renderer::get()->skeleTemp.mCurveTime01) * 5.0f;
		tanLine.startcolor = tanLine.endcolor = glm::vec3(1, 0.5, 0.5);
		Renderer::get()->add_debug_line(tanLine);
		//rotate the animation
		modelToWorld.q = glm::normalize(glm::quatLookAt(tanLine.start - tanLine.end, { 0,1,0 }));
	}
	else modelToWorld.q = glm::quat(1, 0, 0, 0);

	//compute animation times
	aiAnimation* anim = mAnimations[mCurrentAnimation];
	
	double ticksPerSec = (anim->mTicksPerSecond == 0) ? 1 : anim->mTicksPerSecond;
	float durationSec = float(anim->mDuration / ticksPerSec);

	mAnimTime += dt * Globals.animationSpeed;
	if (mAnimTime > anim->mDuration)
		mAnimTime -= (float)anim->mDuration;

	mCurveTime01 += (dt * Globals.animationSpeed) / (Globals.curve.curveLength * Globals.moveAnimateSpeedRatio);
	if (mCurveTime01 > 1.0f)
		mCurveTime01 -= 1.0f;
	
	mAnimTime01 = mAnimTime / (float)anim->mDuration;

	if (Input->IsDown(Keys::B))
		dt = dt + dt - dt;

	//draw
	if (mRootBone != nullptr)
		mRootBone->ComputeAnimationVQS(modelToWorld.toMat4());
	mBoneFound = ik->Compute();
	DebugDraw();

	////////////////////////////

	//position the mesh (TEMP)
	for (auto& mesh : Renderer::get()->Meshes)
	{
		mesh->worldTransform = modelToWorld;
	}
}

void Skeleton::DebugDraw()
{
	if (mRootBone != nullptr)
		mRootBone->DebugDraw(modelToWorld.v);
}

void Skeleton::StartAnimation(int index)
{
	mCurrentAnimation = index;
	mAnimTime = mAnimTime01 = 0.0f;

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
	//initialize members
	mIKquat.w = 1;
	mIKquat.x = mIKquat.y = mIKquat.z = 0;

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
	mAnimTransform = kdn::vqs();

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
	for (unsigned i = 0; i < mNumChildren; i++)
		delete mChildren[i];
}

void Bone::DebugDraw(glm::vec3 parentPosition)
{
	//dont do dumb stuff
	assert(mAiNodeAnim == nullptr || mName.compare(mAiNodeAnim->mNodeName.C_Str()) == 0);

	//interpolate to IK's
	///mAnimTransform.q = glm::slerp(mAnimTransform.q, mIKquat, mSkeleton->mIKfader01);

	//compute worldspace start/end points
	glm::vec3 animPos = glm::vec3(mCompoundTransform * glm::vec4(0, 0, 0, 1.0f));
	glm::vec3 ikPos = glm::vec3(mIKCompoundTransform * glm::vec4(0, 0, 0, 1.0f));
	glm::vec3 myPos = glm::mix(animPos, ikPos, mIKfader01);
	
	//create debug line
	DebugLine dl;
	dl.start = parentPosition;
	dl.end = myPos;
	dl.startcolor = glm::vec3(1, 1, 1);
	dl.endcolor = glm::vec3(1, 1, 1);

	//only draw "real bones"
	if (mName.find("IK") == std::string::npos
		&& mName.compare("Armature")
		&& mName.compare("Circle")
		&& mName.compare("Sphere")
		&& mName.compare("RootNode")
		&& mName.compare("root"))
	{
		Renderer::get()->add_debug_line(dl);
	}

	for (unsigned i = 0; i < mNumChildren; i++)
		mChildren[i]->DebugDraw(myPos);
}

glm::vec3 get(const aiVectorKey& key)
{
	return glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z);
}
glm::quat get(const aiQuatKey& key)
{
	return glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
}
void Bone::ComputeAnimationVQS(glm::mat4& parentCompound)
{
	if (mAiNodeAnim == nullptr)
	{
		//finalize transform data
		mAnimTransform = kdn::vqs();
		mCompoundTransform = parentCompound;

		//propagate
		for (unsigned i = 0; i < mNumChildren; i++)
			mChildren[i]->ComputeAnimationVQS(parentCompound);
		return;
	}

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
	double fader;
	fader = kdn::unterpolate(aiVecA.mTime, aiVecB.mTime, (double)mSkeleton->mAnimTime);
	if (Input->IsDown(Keys::One)) fader = 0;
	glm::vec3 translateKey = glm::mix(get(aiVecA), get(aiVecB), fader);

	fader = kdn::unterpolate(aiQuatA.mTime, aiQuatB.mTime, (double)mSkeleton->mAnimTime);
	if(Input->IsDown(Keys::Two)) fader = 0;
	glm::quat rotateKey = glm::slerp(get(aiQuatA), get(aiQuatB), (float)fader);

	fader = kdn::unterpolate(aiScaleA.mTime, aiScaleB.mTime, (double)mSkeleton->mAnimTime);
	if (Input->IsDown(Keys::Three)) fader = 0;
	glm::vec3 scaleKey = glm::mix(get(aiScaleA), get(aiScaleB), fader);

	if (mName.compare("mixamorig:Hips") == 0) //this is some (TEMP) stuff to freeze mixamo's in place
		translateKey.z = 0;

	//convert to glm/kdn
	mAnimTransform = kdn::vqs(translateKey, rotateKey, scaleKey);

	//finalize transform data
	mCompoundTransform = parentCompound * mAnimTransform.toMat4();

	if (mName.compare("mixamorig:RightHand") == 0)
		scaleKey = scaleKey;
	//propagate
	for (unsigned i = 0; i < mNumChildren; i++)
		mChildren[i]->ComputeAnimationVQS(mCompoundTransform);
}

kdn::vqs Bone::mIKanimVQS()
{
	return kdn::vqs(mAnimTransform.v, mIKquat, mAnimTransform.v);
}






IK::IK(Skeleton & S)
	: s(S)
{
}

bool IK::Compute()
{
	//(TEMP) pick a good default bone
	if (std::string(endEffectorName).length() == 0)
		strcpy_s<IK_BUFFER_SIZE>(endEffectorName, "mixamorig:RightHand");

	//Fix stuff
	depth = glm::max(depth, 0);

	//find bone
	auto endEffector = s.mBoneMap.find(endEffectorName);
	if (endEffector == s.mBoneMap.end())
		return false;

	//Clean those beautiful bones
	Reset();

	//get bone list
	bones.clear();
	Bone* b = endEffector->second;
	for (int d = depth; d >= 0; d--) {
		bones.push_back(b);
		b->mIKfader01 = fader01;
		b = b->mParent;
	}

	//do IK
	for (int attempt = 1; attempt > 0; attempt--)
	{
		for (int i = 1; i <= depth; i++)
		{
			b = bones[i];
			glm::vec3 bonePos = IKBonePos(i);

			glm::vec3 toEnd = kdn::normalize(IKBonePos(0) - bonePos);
			glm::vec3 toTarget = kdn::normalize(targetWorldPoint - bonePos);

			float angle = glm::angle(toEnd, toTarget);
			glm::vec3 axis = kdn::normalize(glm::cross(toEnd, toTarget));

			glm::quat angleAxis = glm::normalize(glm::angleAxis(angle, axis));

			b->mIKquat = angleAxis * b->mIKquat;
		}
	}

	//Ensure all bones know their places
	IKBonePos(-1);

	return true;
}

void IK::Reset()
{
	for (auto& p : s.mBoneMap)
	{
		Bone* b = p.second;
		b->mIKfader01 = 0;
		b->mIKquat = b->mAnimTransform.q;
	}
}

glm::vec3 IK::IKBonePos(int depth)
{
	if (depth == -1)
		depth = (int)bones.size() - 1;

	glm::mat4 compound;
	if (bones[0]->mParent == nullptr)
	{}
	else compound = bones[0]->mParent->mCompoundTransform;

	for (int i = 0; i <= depth; i++)
	{
		compound = compound * bones[i]->mIKanimVQS().toMat4();

		bones[i]->mIKCompoundTransform = compound;
	}

	return glm::vec3(compound * glm::vec4(0, 0, 0, 1.0f));
}
