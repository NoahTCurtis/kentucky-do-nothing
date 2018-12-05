#pragma once

#include <vector>
#include <string>
#include <map>

#include "glm/glm.hpp"
#include "assimp/scene.h"
#include "kdn_math.h"

class Skeleton;
class Bone;

#define IKREQUEST_T_BUFFERSIZE 64
typedef struct
{
	glm::vec3 targetWorldPoint;
	int depth = 3;
	char endEffectorName[IKREQUEST_T_BUFFERSIZE] = { 0 };
} IKrequest_t;

class IK
{

};

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
	bool IK(IKrequest_t& request);
	void ResetAllIKData();
	float mIKfader01 = 1.0f;
	std::string mLastIKBoneName; //for convenience

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
	void DebugDraw(glm::mat4& parentCompound);
	void ComputeAnimationVQS();

	//Inverse Kinematics
	void ComputeFullIKMove(glm::vec3 worldPoint, int recurseDepth);
	bool ComputeSingleIKMove(glm::vec3 worldPoint, int recurseDepth, bool isEndEffector = false);
	std::pair<bool, glm::vec3> EndEffectorPosition(glm::mat4 parentCompound);
	kdn::vqs getIKanimVQS();
	///glm::vec3 getWorldSpacePoint();
	glm::quat mIKquat;
	bool amEndEffector = false;

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
