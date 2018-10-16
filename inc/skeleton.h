#pragma once

#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "assimp/scene.h"

class Skeleton;
class Bone;


class Bone
{
	friend class Skeleton;
public:
	Bone(const aiNode* node, Bone* parent);
	~Bone();

	std::string mName;
	Bone* mParent;
	glm::mat4 mTransform;

	unsigned mNumChildren;
	std::vector<Bone*> mChildren;

	/** The number of meshes of this node. */
	unsigned mNumMeshes;
	/** The meshes of this node. Each entry is an index into the mesh */
	std::vector<unsigned> mMeshes;
};


class Skeleton
{
	friend class Bone;
public:
	Skeleton(const aiScene* scene);
	~Skeleton();

	Bone* root;
};


