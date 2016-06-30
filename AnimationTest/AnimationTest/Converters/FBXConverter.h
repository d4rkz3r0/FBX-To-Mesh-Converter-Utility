#pragma once
#include "fbxsdk.h"
#include "../CommonIncludes.h"
#include "Vertex.h"
#include <vector>
#include <string>

struct animated_keyframe
{
	float currentTime;
	uint32_t frameNumber;

	XMFLOAT4X4 globalTransform;
};

struct animated_bone
{
	string name;

	bool hasAnimationData;
	int32_t parentIndex;
	uint32_t numKeyFrames;

	vector<animated_keyframe> vKeyframe;

	XMFLOAT4X4 bindPoseInverse;
	XMFLOAT4X4 localMatrix;
};

struct animated_skeleton
{
	vector<animated_bone> bones;
};

struct animated_animation
{
	string name;
	float duration;
};

class FBXConverter
{
public:
	FBXConverter();
	~FBXConverter();

	unsigned int Convert(char* _filename);

private:
	/*FbxMesh*						m_fbxMesh;
	std::vector<animated_vert*>		m_vVerts;*/

	unsigned int Read(char* _filename);
	unsigned int Write(char* _filename, animated_animation& _animation, vector<animated_vert>& _vVertex, animated_skeleton& _skeleton);

	void ExtractModel(FbxMesh* _pMesh, vector<animated_vert>& _vVertex);
	void ExtractControlPoints(FbxMesh* _pMesh, vector<XMFLOAT4>& _vControlPoints);
	void ExtractVertexInformation(FbxMesh* _pMesh, vector<animated_vert>& _vVertex, vector<XMFLOAT4>& _vControlPoints);
	void ExtractSkeleton(FbxMesh* _pMesh, FbxScene* _pScene, animated_skeleton& _skeleton);
	void ExtractAnimations(FbxMesh* _pMesh, FbxScene* _pScene, animated_animation& _animation, vector<animated_vert>& _vVertex, animated_skeleton& _skeleton);
};

