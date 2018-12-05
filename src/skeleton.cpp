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
	DebugDraw();

	////////////////////////////

	//position the mesh
	for (auto& mesh : Renderer::get()->Meshes)
	{
		mesh->worldTransform = modelToWorld;
	}
}

void Skeleton::DebugDraw()
{
	if (mRootBone != nullptr)
		mRootBone->DebugDraw(modelToWorld.toMat4());
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

bool Skeleton::IK(IKrequest_t& request)
{
	//(TEMP)
	if (std::string(request.endEffectorName).length() == 0)
		strcpy_s<IKREQUEST_T_BUFFERSIZE>(request.endEffectorName, "mixamorig:RightHand");

	//find bone
	auto endEffector = mBoneMap.find(request.endEffectorName);
	if (endEffector == mBoneMap.end())
		return false;
	mLastIKBoneName = request.endEffectorName;

	//perform IK request
	endEffector->second->ComputeFullIKMove(request.targetWorldPoint, request.depth);
	return true;
}

void Skeleton::ResetAllIKData()
{
	std::stack<Bone*> bones;
	bones.push(mRootBone);
	while (!bones.empty())
	{
		Bone* bone = bones.top();
		bones.pop();
		for (unsigned i = 0; i < bone->mNumChildren; i++)
			bones.push(bone->mChildren[i]);

		bone->amEndEffector = false;
		bone->mIKquat.w = 1;
		bone->mIKquat.x = bone->mIKquat.y = bone->mIKquat.z = 0;
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

void Bone::DebugDraw(glm::mat4& parentCompound)
{
	//dont do dumb stuff
	assert(mAiNodeAnim == nullptr || mName.compare(mAiNodeAnim->mNodeName.C_Str()) == 0);

	//extract animation data
	ComputeAnimationVQS();

	//interpolate to IK's
	mAnimTransform.q = glm::slerp(mAnimTransform.q, mIKquat, mSkeleton->mIKfader01);

	//compute worldspace start/end points
	mCompoundTransform = parentCompound * mAnimTransform.toMat4();
	glm::vec3 startpos = glm::vec3(parentCompound * glm::vec4(0, 0, 0, 1.0f));
	glm::vec3 endpos = glm::vec3(mCompoundTransform * glm::vec4(0, 0, 0, 1.0f));
	
	//create debug line
	DebugLine dl;
	dl.start = startpos;
	dl.end = endpos;
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

	//TELL ME! TELL ME ABOUT THE BONESSSS!!!
	if (Input->IsTriggered(Keys::B))
	{
		if(mName.compare("RootNode") == 0)
			std::cout << "\n\n\n";
		std::cout
			<< "["
			<< glm::distance(dl.start, dl.end)
			<< "] "
			<< mName
			<< " drew a line from ("
			<< dl.start.x
			<< ","
			<< dl.start.y
			<< ","
			<< dl.start.z
			<< ") to ("
			<< dl.end.x
			<< ","
			<< dl.end.y
			<< ","
			<< dl.end.z
			<< ")\n";
	}

	///std::cout << mName << ": (" << dl.start.x << ", " << dl.start.y << ", " << dl.start.z << ") -> ("
	///	<< dl.end.x << ", " << dl.end.y << ", " << dl.end.z << ")\n";

	for (unsigned i = 0; i < mNumChildren; i++)
		mChildren[i]->DebugDraw(mCompoundTransform);
}

glm::vec3 get(const aiVectorKey& key)
{
	return glm::vec3(key.mValue.x, key.mValue.y, key.mValue.z);
}
glm::quat get(const aiQuatKey& key)
{
	return glm::quat(key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
}
void Bone::ComputeAnimationVQS()
{
	if (mAiNodeAnim == nullptr)
	{
		mAnimTransform = kdn::vqs();
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
	//convert to glm/kdn
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

	//this is some (TEMP) stuff to freeze mixamo's in place
	if (mName.compare("mixamorig:Hips") == 0)
		translateKey.z = 0;

	mAnimTransform = kdn::vqs(translateKey, rotateKey, scaleKey);
}

void Bone::ComputeFullIKMove(glm::vec3 targetWorldPoint, int recurseDepth)
{
	int tries = 1;
	while (tries--)
		if (!ComputeSingleIKMove(targetWorldPoint, recurseDepth, true))
			return;
}

bool Bone::ComputeSingleIKMove(glm::vec3 targetWorldPoint, int recurseDepth, bool isEndEffector)
{
	if (this == nullptr) return false;
	if (recurseDepth == 0 && !isEndEffector) return false;

	glm::vec3 jointPos = getWorldSpacePoint();

	if (isEndEffector)
	{
		std::cout << mName << " (" << jointPos.x << ", " << jointPos.y << ", " << jointPos.z << ")" << std::endl;
		return mParent->ComputeSingleIKMove(targetWorldPoint, recurseDepth);
	}
	else
	{
		bool moveMade = false;

		glm::vec3 toEndEffector = endEffectorWorldPoint - jointPos;
		glm::vec3 toTarget = targetWorldPoint - jointPos;

		float angle = glm::angle(toEndEffector, toTarget);

		glm::vec3 axis = glm::cross(toEndEffector, toTarget);

		mIKquat = glm::angleAxis(mSkeleton->mIKfader01 * angle, axis); //should multiply instead of setting

		std::cout << mName << std::endl;
		return mParent->ComputeSingleIKMove(targetWorldPoint, recurseDepth - 1) || moveMade;
	}
}

std::pair<bool, glm::vec3> Bone::EndEffectorPosition(glm::mat4 parentCompound)
{
	return std::pair<bool, glm::vec3>();
}

kdn::vqs Bone::getIKanimVQS()
{
	return kdn::vqs(mAnimTransform.v, mIKquat, mAnimTransform.s);
}

///glm::vec3 Bone::getWorldSpacePoint()
///{
///	///return glm::vec3(getIKanimVQS * glm::vec4(0, 0, 0, 1.0f));
///	return 
///}
