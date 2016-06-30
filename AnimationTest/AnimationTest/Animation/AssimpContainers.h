#pragma once
#include "../CommonIncludes.h"
#include "ImporterHeaders.h"

/*
Custom Classes that mirror Assimp's aiStructs and catch the data
output by the aiScene object.
*/

class Bone
{
public:
	string Name;
	XMFLOAT4X4 Offset;
	XMFLOAT4X4 LocalTransform;
	XMFLOAT4X4 GlobalTransform;
	XMFLOAT4X4 OriginalLocalTransform;
	Bone* Parent;
	vector<Bone*> Children;

	Bone() : Parent(nullptr) { }

	~Bone()
	{
		for (size_t i(0); i < Children.size(); i++)
		{
			delete Children[i];
		}
	}
};


class AnimationChannel
{
public:
	string Name;
	vector<aiVectorKey> mPositionKeys;
	vector<aiQuatKey> mRotationKeys;
	vector<aiVectorKey> mScalingKeys;
};


class AnimationEvaluator
{
public:

	AnimationEvaluator() : AnimationIndex(0), PlayAnimationForward(true), mLastTime(0.0f), TicksPerSecond(0.0f), Duration(0.0f) { }
	AnimationEvaluator(const aiAnimation* pAnim);
	void Evaluate(float pTime, map<string, Bone*>& bones);
	void Save(std::ofstream& file);
	void Load(std::ifstream& file);
	vector<XMFLOAT4X4>& GetTransforms(float deltaTime) { return Transforms[GetFrameIndexAt(deltaTime)]; }
	unsigned int GetFrameIndexAt(float time);

	string Name;
	std::uint32_t AnimationIndex;
	vector<AnimationChannel> Channels;
	bool PlayAnimationForward;
	float mLastTime, TicksPerSecond, Duration;
	vector<tuple<unsigned int, unsigned int, unsigned int>> mLastPositions;
	vector<vector<XMFLOAT4X4>> Transforms;
};


class SceneAnimator
{
public:

	SceneAnimator() : CurrentAnimIndex(-1), Skeleton(nullptr) {}
	~SceneAnimator() { Release(); }

	void Init(const aiScene* pScene);
	void Release();
	void Save(ofstream& file);
	void Load(ifstream& file);
	void PlayAnimationForward() { Animations[CurrentAnimIndex].PlayAnimationForward = true; }
	void PlayAnimationBackward() { Animations[CurrentAnimIndex].PlayAnimationForward = false; }
	void AdjustAnimationSpeedBy(float percent) { Animations[CurrentAnimIndex].TicksPerSecond *= percent / 100.0f; }
	void AdjustAnimationSpeedTo(float tickspersecond) { Animations[CurrentAnimIndex].TicksPerSecond = tickspersecond; }

	float GetAnimationSpeed() const { return Animations[CurrentAnimIndex].TicksPerSecond; }
	vector<XMFLOAT4X4>& GetTransforms(float deltaTime) { return Animations[CurrentAnimIndex].GetTransforms(deltaTime); }
	std::int32_t GetAnimationIndex() const { return CurrentAnimIndex; }
	string GetAnimationName() const { return Animations[CurrentAnimIndex].Name; }
	int GetBoneIndex(const string& boneName) { map<string, unsigned int>::iterator found = BonesToIndex.find(boneName); if (found != BonesToIndex.end()) return found->second; else return -1; }
	XMFLOAT4X4 GetBoneTransform(float deltaTime, const string& boneName) { int boneIndex = GetBoneIndex(boneName); if (boneIndex == -1) return XMFLOAT4X4(); return Animations[CurrentAnimIndex].GetTransforms(deltaTime)[boneIndex]; }
	XMFLOAT4X4 GetBoneTransform(float deltaTime, unsigned int boneIndex) { return Animations[CurrentAnimIndex].GetTransforms(deltaTime)[boneIndex]; }

	bool HasSkeleton() const { return !Bones.empty(); }
	bool SetAnimIndex(std::int32_t indexValue);
	bool SetAnimation(const string& name);

	vector<AnimationEvaluator> Animations;
	std::int32_t CurrentAnimIndex;

protected:
	Bone* Skeleton;
	map<string, Bone*> BonesByName;
	map<string, unsigned int> BonesToIndex;
	map<string, std::uint32_t> AnimationNameToId;
	vector<Bone*> Bones; //ALL the Model's Bones
	vector<XMFLOAT4X4> Transforms;

	void SaveSkeleton(std::ofstream& file, Bone* pNode);
	Bone* LoadSkeleton(std::ifstream& file, Bone* pNode);

	void UpdateTransforms(Bone* pNode);
	void CalculateBoneToWorldTransform(Bone* pInternalNode);

	void Calculate(float pTime);

	void ExtractAnimations(const aiScene* pScene);
	Bone* CreateBoneTree(aiNode* pNode, Bone* pParent);
};