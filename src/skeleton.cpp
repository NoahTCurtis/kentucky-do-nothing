#include <iostream>

#include "skeleton.h"

Skeleton::Skeleton(const aiScene* scene)
{
	root = new Bone(scene->mRootNode, nullptr);
}

Skeleton::~Skeleton()
{
	//SHOULD TELL ROOT BONE TO DESTORY ITS CHILDREN
}

Bone::Bone(const aiNode* node, Bone* parent)
{
	//copy data
	std::memcpy(&mTransform, &node->mTransformation, 16 * sizeof(float));
	mName = std::string(node->mName.C_Str());
	mParent = parent;


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
