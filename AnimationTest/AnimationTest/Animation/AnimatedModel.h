#pragma once
#include "../CommonIncludes.h"
#include "VertexLibrary.h"
#include "CBufferLayout.h"
#include "AssimpContainers.h"
#include "ConstantBuffer.h"
#include <CommonStates.h>
using std::vector;

//Hack Includes until Renderer rewrite.
#include "AnimatedShaderRefs.h"
#include "VertexBufferWrapper.h"
#include "IndexBufferWrapper.h"


//Forward Declarations
class SceneAnimator;

class AnimatedModel
{
public:
	AnimatedModel() = default;
	~AnimatedModel();
	void Initialize(ID3D11Device* theDevice, ID3D11DeviceContext* theDeviceContext);
	void LoadSkinnedModel(const string& fileName);
	//Interface between GameObject's Animation Component and the GameObject it Controls

	//Sends Bone Transformations to the GPU via Constant Buffer
	void Update(float elapsedTime, CXMMATRIX ObjectWorld);
	void UpdateConstantBuffers(float elapsedTime, CXMMATRIX ObjectWorld);
	void SetElapsedTime(float elapsedTime);

	void DrawMe(float elapsedTime, CXMMATRIX ObjectWorld);

	//Can be used to hotswap animation components between GameObjects @ runtime.
	void SetAnimator(SceneAnimator* animator);

	//-PlayBack Controls-//
	//Start Playing an animation by string or if you know the index.
	void SetAnimation(string animation);
	void SetAnimation(int index);
	//Speed or Slow up the Current animation
	void AdjustAnimationSpeedBy(float percent);

	//-Accessors-//
	//Playback Info
	int GetCurrentAnimation();
	float GetAnimationSpeed();
	size_t GetNumAnimations();

	//Skinning Transforms
	vector<XMFLOAT4X4> GetFinalTransforms();

	//Model's Material Info
	cbPerObjectMaterial GetMaterial();
	void LoadMaterialScalars(AnimatedModel* aModel, const aiMaterial* meshMaterial);
	void LoadTextures(AnimatedModel* aModel);

	//Misc
	void SetFilename(string filename);
	string GetFilename();

	//Drawing Accessors/Helpers
	D3D11_PRIMITIVE_TOPOLOGY GetModelTopology() { return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; }
	vector<AnimationVertex>& GetVertices();
	vector<uint16_t>& GetIndicies();
	VertexBufferWrapper<AnimationVertex>& GetVertexBuffer();
	IndexBufferWrapper& GetIndexBuffer();
	vector<AnimationVertex> mVertices;
	vector<uint16_t> mIndices;
	UINT mModelNumVertices;
	UINT mModelNumIndices;
	UINT mModelStartingIndex;

	//Helper Funcs - One-Time Wrappers
	//Initialization 
	void BuildAnimatedModelPipelineParams();

	//D3D Objects
	ID3D11Device* mDirect3DDevice;
	ID3D11DeviceContext* mDirect3DDeviceContext;


	static float mElapsedTime;

	//Animation Encapsulation
	unique_ptr<SceneAnimator> mAnimator;

	//Animated Model Buffers
	VertexBufferWrapper<AnimationVertex> mModelVertexBuffer;
	IndexBufferWrapper mModelIndexBuffer;
	ConstantBuffer<cbPerObjectMatrix> mAnimatedModelMatrixInfo;
	ConstantBuffer<cbPerObjectMaterial> mAnimatedModelMaterialInfo;
	ConstantBuffer<cbPerObjectSkinningData> mCalculatedBoneTransforms;

	//D3D Misc
	UINT mIBStride;
	unique_ptr<CommonStates> states;
	ID3D11SamplerState* mAnisoSamplerState;

	//Shaders
	ID3D11VertexShader* mAnimVertexShader;
	ID3D11PixelShader* mAnimPixelShader;

	//Texture SRVs
	ID3D11ShaderResourceView* mModelDiffuseTexture;
	ID3D11ShaderResourceView* mModelNormalMapTexture;

	//File Info
	string mFilename;
	string mFileType;
	string mFilePath;

	string mModelFileFullPath;
	string mDiffuseTextureFilePath;
	string mNormalMapTextureFilePath;

	string mDefaultModelFilePath = "Assets\\Models\\Actors\\";
	string mDefaultTextureFilePath = "Assets\\Textures\\Actors\\";
	string mDiffuseTextureFileExtension = "_D.png";
	string mNormalMapTextureFileExtension = "_N.png";

	cbPerObjectMaterial mMaterialInfo;
	ID3D11InputLayout* mAnimatedMeshInputLayout;
	D3D11_PRIMITIVE_TOPOLOGY mModelTopology;

	void Save(ofstream& fout);
	void Load(ifstream& fin);
};