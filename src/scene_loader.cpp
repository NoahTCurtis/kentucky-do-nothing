

//////////////////////////////////////////////////////////////////////
// Uses the ASSIMP library to read an animation file and print most of
// the relevant information.
////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <cassert>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "scene_loader.h"
#include "mesh.h"
#include "vertex.h"
#include "render.h"
#include "util.h"
#include "skeleton.h"

const int MAX = 3; // Loops will print at most MAX entries followed by an elipsis.

// Prints a mesh's info; A mesh contains vertices, faces, normals and
// more as needed for graphics.  Vertices are tied to bones with
// weights.
void showMesh(aiMesh* mesh)
{
	// Mesh name and some counts
	printf("Mesh %s: %d vertices, %d faces,  %d bones\n", mesh->mName.C_Str(),
		mesh->mNumVertices, mesh->mNumFaces, mesh->mNumBones);

	// Mesh's bones and weights of all connected vertices.
	for (int i = 0; i < (int)mesh->mNumBones && i < MAX; i++) {
		aiBone* bone = mesh->mBones[i];
		printf("  %s:  %d weights;  OffsetMatrix:[%f, ...]\n", bone->mName.C_Str(),
			bone->mNumWeights, bone->mOffsetMatrix[0][0]);
		for (int i = 0; i < (int)bone->mNumWeights && i < MAX; i++) {
			printf("    %d %f\n", bone->mWeights[i].mVertexId, bone->mWeights[i].mWeight);
		}
		if (bone->mNumWeights > MAX) printf("    ...\n");
	}
	if (mesh->mNumBones > MAX) printf("  ...\n");
}

Mesh* convert_aimesh_to_kdnmesh(aiMesh* aimesh)
{
	//Make a mesh for the graphics system
	Mesh* kdnMesh = new Mesh();
	kdnMesh->name = std::string(aimesh->mName.C_Str());
	kdnMesh->aimesh = aimesh;

	std::vector<Vertex> kdnVertices;
	kdnVertices.reserve(aimesh->mNumVertices);
	std::vector<unsigned> kdnIndices;
	kdnIndices.reserve(aimesh->mNumFaces * 3);

	Vertex vert;
	for (int i = 0; i < (int)aimesh->mNumVertices; i++)
	{
		vert.position.x = aimesh->mVertices[i].x;
		vert.position.y = aimesh->mVertices[i].y;
		vert.position.z = aimesh->mVertices[i].z;
		kdnVertices.push_back(vert);
	}
	for (int i = 0; i < (int)aimesh->mNumFaces; i++)
	{
		assert(aimesh->mFaces[i].mNumIndices == 3);
		kdnIndices.push_back(aimesh->mFaces[i].mIndices[0]);
		kdnIndices.push_back(aimesh->mFaces[i].mIndices[1]);
		kdnIndices.push_back(aimesh->mFaces[i].mIndices[2]);
	}

	kdnMesh->randomize_vertex_colors(kdnVertices);
	kdnMesh->create_VAO_from_raw_data(kdnVertices, kdnIndices);
	return kdnMesh;
}

// Prints an animation.  An animation contains a few timing parameters
// and then channels for a number of animated bones.  Each channel
// contains a V, Q, and S keyframe sequences.
void showAnimation(aiAnimation* anim)
{
	printf("Animation: %s\n  duration (in ticks): %f\n  tickspersecond: %f\n  numchannels: %d\n",
		anim->mName.C_Str(),
		anim->mDuration,
		anim->mTicksPerSecond,
		anim->mNumChannels);

	// The animations channels
	for (int i = 0; i < (int)anim->mNumChannels && i < MAX; i++) {
		aiNodeAnim* chan = anim->mChannels[i];

		// Prints the bone name followed by the numbers of each key type
		printf("\n");
		printf("    %-15s VQS keys:  %d %d %d\n",
			chan->mNodeName.C_Str(),
			chan->mNumPositionKeys,
			chan->mNumRotationKeys,
			chan->mNumScalingKeys);

		continue;//REMOVE THIS (TEMP) LINE

		// The position (V) keys
		printf("\n");
		for (int i = 0; i < (int)chan->mNumPositionKeys && i < MAX; i++) {
			aiVectorKey key = chan->mPositionKeys[i];
			printf("      V[%d]: %f : (%f %f %f)\n", i, key.mTime,
				key.mValue[0], key.mValue[1], key.mValue[2]);
		}
		if (chan->mNumPositionKeys > MAX) printf("      ...\n");

		// The rotation (Q) keys
		printf("\n");
		for (int i = 0; i < (int)chan->mNumRotationKeys && i < MAX; i++) {
			aiQuatKey key = chan->mRotationKeys[i];
			printf("      Q[%d]: %f : (%f %f %f %f)\n", i, key.mTime,
				key.mValue.w, key.mValue.x, key.mValue.y, key.mValue.z);
		}
		if (chan->mNumRotationKeys > MAX) printf("      ...\n");

		// the scaling (S) keys
		printf("\n");
		for (int i = 0; i < (int)chan->mNumScalingKeys && i < MAX; i++) {
			aiVectorKey key = chan->mScalingKeys[i];
			printf("      S[%d]: %f : (%f %f %f)\n", i, key.mTime,
				key.mValue[0], key.mValue[1], key.mValue[2]);
		}
		if (chan->mNumScalingKeys > MAX) printf("      ...\n");
	}

	if (anim->mNumChannels > MAX) printf("    ...\n");
}

// Prints the bone hierarchy and relevant info with a graphical
// representation of the hierarchy.
void showBoneHierarchy(const aiScene* scene, const aiNode* node, const int level)
{
	// Print indentation to show this node's level in the hierarchy
	for (int i = 0; i < level; i++)
		printf(" |");

	// Print node name and transformation to parent's node
	printf("%s                    Transformation:[%f, ...]\n", node->mName.C_Str(),
		node->mTransformation[0][0]);

	// Recurse onto this node's children
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
		showBoneHierarchy(scene, node->mChildren[i], level + 1);
}


void ReadAssimpFile(const std::string& path)
{
	printf("Reading %s\n", path.c_str());

	// A single call returning a single structure for the complete file.
	const aiScene* scene = Renderer::get()->scene = Renderer::get()->importer.ReadFile(path.c_str(),
		aiProcess_Triangulate | aiProcess_GenNormals);

	printf("  %d animations\n", scene->mNumAnimations); // This is what 460/560 is all about
	printf("  %d meshes\n", scene->mNumMeshes);         // Verts and faces for the skin.
	printf("  %d materials\n", scene->mNumMaterials);   // More graphics info
	printf("  %d textures\n", scene->mNumTextures);     // More graphics info
	printf("\n");

	// Prints a graphical representation of the bone hierarchy.
	showBoneHierarchy(scene, scene->mRootNode);

	// Prints all the animation info for each animation in the file
	printf("\n");
	for (int i = 0; i < (int)scene->mNumAnimations; i++)
		showAnimation(scene->mAnimations[i]);

	// Prints all the mesh info for each mesh in the file
	printf("\n");
	for (int i = 0; i < (int)scene->mNumMeshes; i++)
	{
		showMesh(scene->mMeshes[i]);
		Renderer::get()->add_mesh(convert_aimesh_to_kdnmesh(scene->mMeshes[i]));
	}

	Renderer::get()->skeleTemp.Initialize(scene);
}