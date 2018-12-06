#pragma once

#include <vector>
#include <string>
#include <map>

#include "glm/glm.hpp"
#include "assimp/scene.h"
#include "kdn_math.h"

class Skeleton;
class Bone;
class IK;

class Skeleton
{
	friend class Bone;
	friend class IK;
public:
	Skeleton();
	void Initialize(const aiScene* scene);
	~Skeleton();

	void Update(float dt);
	void DebugDraw();
	void StartAnimation(int index);

	std::vector<aiAnimation*> mAnimations;
	float mAnimTime01 = 0.0f; //[0,1] progress through animation
	float mAnimTime = 0.0f; //scaled progress through animation
	float mCurveTime01 = 0.0f;

	//Inverse Kinematics
	IK* ik;
	bool mBoneFound;

	kdn::vqs modelToWorld; //Transform component
private:
	Bone* mRootBone;
	std::map<std::string, Bone*> mBoneMap;
	int mCurrentAnimation = 0;

	int calcium; //why not
};


class Bone
{
	friend class Skeleton;
	friend class IK;
public:
	Bone(const aiNode* node, Bone* parent);
	~Bone();
	void DebugDraw(glm::vec3 parentPosition);
	void ComputeAnimationVQS(glm::mat4& parentCompound);

	//Inverse Kinematics
	kdn::vqs mIKanimVQS();
	glm::quat mIKquat;
	float mIKfader01;
	glm::mat4 mIKCompoundTransform;

	//structural data
	std::string mName;
	Skeleton* mSkeleton = nullptr;
	Bone* mParent = nullptr;

	//mathematical data
	glm::mat4 mTransform; //boneSpace to parentBoneSpace (bindpose)
	glm::mat4 mCompoundTransform; //boneSpace to modelSpace (or to worldspace if started with model2world)

	//children
	unsigned mNumChildren = -1;
	std::vector<Bone*> mChildren;

	//animation data
	aiNodeAnim* mAiNodeAnim = nullptr;
	kdn::vqs mAnimTransform;
	

	/** The number of meshes of this node. */
	///unsigned mNumMeshes;
	/** The meshes of this node. Each entry is an index into the mesh */
	///std::vector<unsigned> mMeshes;
};


#define IK_BUFFER_SIZE 64
class IK
{
public:
	IK(Skeleton& S);
	bool Compute();
	void Reset();
	glm::vec3 IKBonePos(int depth);

	Skeleton& s;

	glm::vec3 targetWorldPoint = glm::vec3(0);
	int depth = 3;
	char endEffectorName[IK_BUFFER_SIZE] = { 0 };
	std::vector<Bone*> bones;
	float fader01 = 1.0f;
};