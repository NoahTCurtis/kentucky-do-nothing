#pragma once

#include <vector>
#include <string>
#include <map>

#include "glm/glm.hpp"
#include "assimp/scene.h"
#include "kdn_math.h"

class Skeleton;
class Bone;


class Skeleton
{
	friend class Bone;
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
public:
	Bone(const aiNode* node, Bone* parent);
	~Bone();
	void DebugDraw(glm::mat4& parentCompound);
	void ComputeAnimationVQS();

	//structural data
	std::string mName;
	Skeleton* mSkeleton = nullptr;
	Bone* mParent = nullptr;

	//mathematical data
	glm::mat4 mTransform; //boneSpace to parentBoneSpace
	glm::mat4 mCompoundTransform; //boneSpace to modelSpace

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
