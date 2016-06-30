
#include "AnimatedModel.h"
#include "CBufferLayout.h"
#include "../ThirdPersonCamera.h"

float AnimatedModel::mElapsedTime = 0.0f;

namespace Utility
{
	string GetFileExtension(const string& s)
	{
		size_t i = s.rfind('.', s.length());

		if (i != string::npos)
		{
			return(s.substr(i + 1, s.length() - i));
		}

		return("");
	}

	string tail(string const& source, size_t const length)
	{
		if (length >= source.size())
		{
			return source;
		}
		return source.substr(source.size() - length);
	}

	wstring widen(const string& str)
	{
		wostringstream wstm;
		const ctype<wchar_t>& ctfacet =
			use_facet< ctype<wchar_t> >(wstm.getloc());
		for (size_t i = 0; i < str.size(); ++i)
			wstm << ctfacet.widen(str[i]);
		return wstm.str();
	}
}

enum class FILE_TYPE
{
	FBX_FILE = 'x'
};

AnimatedModel::~AnimatedModel()
{
	mModelVertexBuffer.Destroy();
	mModelIndexBuffer.Destroy();
	mAnimatedModelMatrixInfo.ShutDown();
	mAnimatedModelMaterialInfo.ShutDown();
	mCalculatedBoneTransforms.ShutDown();
	ReleaseObject(mAnimVertexShader);
	ReleaseObject(mAnimPixelShader);
	ReleaseObject(mModelDiffuseTexture);
	ReleaseObject(mModelNormalMapTexture);
	ReleaseObject(mAnisoSamplerState);
}

void AnimatedModel::Initialize(ID3D11Device* theDevice, ID3D11DeviceContext* theDeviceContext)
{
	mDirect3DDevice = theDevice;
	mDirect3DDeviceContext = theDeviceContext;
	mAnimator.reset(new SceneAnimator());
	BuildAnimatedModelPipelineParams();

	//All Buffers
	mModelVertexBuffer.Initialize(theDevice);
	mModelIndexBuffer.Initialize(theDevice);
	mAnimatedModelMatrixInfo.Initialize(theDevice);
	mAnimatedModelMaterialInfo.Initialize(theDevice);
	mCalculatedBoneTransforms.Initialize(theDevice);
}

void AnimatedModel::LoadSkinnedModel(const string& fileName)
{
	//Cache Mesh Info
	mFilename = fileName;

	ifstream vertexBinaryInfo("exampleVertex.bin", ios::binary);
	Load(vertexBinaryInfo);
	
	////Assimp Load
	//UINT importFlags = aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded;
	//const aiScene* scene = aiImportFile(fileName.c_str(), importFlags);
	//
	//if (scene == nullptr)
	//{
	//	throw Exception("AnimatedMesh::LoadModel(), File not found!");
	//}
	//
	////Init
	//size_t numVertices(0);
	//size_t currVertex(0);
	//size_t numIndices(0);
	//size_t currIndex(0);
	//bool bonesPresent = false;
	//
	////Obtain Vertex and Indices Count
	//for (size_t i = 0; i < scene->mNumMeshes; i++)
	//{
	//	numVertices += scene->mMeshes[i]->mNumVertices;
	//	numIndices += scene->mMeshes[i]->mNumFaces * 3;
	//	if (scene->mMeshes[i]->HasBones())
	//	{
	//		bonesPresent = true;
	//	}
	//}
	////Early Out
	//if (!bonesPresent)
	//{
	//	aiReleaseImport(scene);
	//	throw Exception("AnimatedMesh::LoadModel(), No Bones detected!");
	//}
	//
	////Determine Index Buffers ByteWidth
	//if (numVertices >= 65536)
	//{
	//	mModelIndexBuffer.SetStride(4);
	//}
	//
	////Vector Prep
	//mVertices.resize(numVertices);
	//mIndices.resize((mModelIndexBuffer.GetStride() / 2) * numIndices);
	//vector<string> boneStrings;
	//
	//for (size_t i = 0; i < scene->mNumMeshes; i++)
	//{
	//	//Validate MeshFile
	//	const aiMesh* mesh = scene->mMeshes[i];
	//	if (mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
	//	{
	//		throw Exception("AnimatedMesh::LoadModel(), MeshFile's Topology doesn't consist of Triangles!");
	//	}
	//	if (!mesh->HasTextureCoords(0))
	//	{
	//		throw Exception("AnimatedMesh::LoadModel(), MeshFile lacks UVs!");
	//	}
	//	if (!mesh->HasTangentsAndBitangents())
	//	{
	//		throw Exception("AnimatedMesh::LoadModel(), MeshFile lacks Tangent Info!");
	//	}
	//
	//	//Cache Mesh Info
	//	mModelNumVertices = mesh->mNumVertices;
	//	mModelNumIndices = mesh->mNumFaces * 3;
	//	mModelStartingIndex = static_cast<std::uint32_t>(currIndex);
	//
	//	//Populate Vertices
	//	for (size_t x = 0; x < mesh->mNumVertices; x++)
	//	{
	//		//Position
	//		mVertices[x + currVertex].Position = *reinterpret_cast<XMFLOAT3*>(&mesh->mVertices[x]);
	//		//UVs
	//		mVertices[x + currVertex].TextureCoordinates = *reinterpret_cast<XMFLOAT2*>(&mesh->mTextureCoords[0][x]);
	//		//Normals
	//		mVertices[x + currVertex].Normal = *reinterpret_cast<XMFLOAT3*>(&mesh->mNormals[x]);
	//		//Tangents
	//		mVertices[x + currVertex].Tangent = *reinterpret_cast<XMFLOAT3*>(&mesh->mTangents[x]);
	//	}
	//
	//	//Populate Indices Based on ByteWidth
	//	if (mModelIndexBuffer.GetStride() == 4)
	//	{
	//		std::uint32_t* indexBufferData = reinterpret_cast<std::uint32_t*>(&mIndices[currIndex]);
	//		for (size_t x = 0; x < mesh->mNumFaces; ++x)
	//		{
	//			for (size_t a = 0; a < 3; ++a)
	//			{
	//				*indexBufferData++ = static_cast<std::uint32_t>(mesh->mFaces[x].mIndices[a] + currVertex);
	//			}
	//		}
	//	}
	//	else
	//	{
	//		uint16_t* indexBufferData = reinterpret_cast<uint16_t*>(&mIndices[currIndex]);
	//		for (size_t x = 0; x < mesh->mNumFaces; ++x)
	//		{
	//			for (size_t a = 0; a < 3; ++a)
	//			{
	//				*indexBufferData++ = static_cast<uint16_t>(mesh->mFaces[x].mIndices[a] + currVertex);
	//			}
	//		}
	//	}
	//
	//	//Iterate Through Mesh Files Bones
	//	for (size_t a = 0; a < mesh->mNumBones; a++)
	//	{
	//		//Init
	//		const aiBone* currBone = mesh->mBones[a];
	//		size_t boneIndex = -1;
	//	
	//		//Check for unique bone names
	//		for (size_t bIndex = 0; bIndex < boneStrings.size(); bIndex++)
	//		{
	//			string boneName = currBone->mName.data;
	//			if (boneName == boneStrings[bIndex])
	//			{
	//				boneIndex = bIndex;
	//				break;
	//			}
	//		}
	//	
	//		//Add unique bone name
	//		if (boneIndex == -1)
	//		{
	//			boneIndex = boneStrings.size();
	//			boneStrings.push_back(currBone->mName.data);
	//		}
	//	
	//		//Extract Bone Indices and their Weights
	//		for (size_t b = 0; b < currBone->mNumWeights; b++)
	//		{
	//			if (mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.x <= 0.f)
	//			{
	//				mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneIndices.x = static_cast<float>(boneIndex);
	//				mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.x = currBone->mWeights[b].mWeight;
	//			}
	//			else if (mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.y <= 0.f)
	//			{
	//				mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneIndices.y = static_cast<float>(boneIndex);
	//				mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.y = currBone->mWeights[b].mWeight;
	//			}
	//			else if (mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.z <= 0.f)
	//			{
	//				mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneIndices.z = static_cast<float>(boneIndex);
	//				mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.z = currBone->mWeights[b].mWeight;
	//			}
	//			else if (mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.w <= 0.f)
	//			{
	//				mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneIndices.w = static_cast<float>(boneIndex);
	//				mVertices[currBone->mWeights[b].mVertexId + currVertex].BoneWeights.w = currBone->mWeights[b].mWeight;
	//			}
	//		}
	//	}
	//
	//	//Load Textures & Get Material Info
	//	LoadTextures(this);
	//	LoadMaterialScalars(this, scene->mMaterials[mesh->mMaterialIndex]);
	//
	//	//Continue
	//	currVertex += mesh->mNumVertices;
	//	currIndex += mesh->mNumFaces * 3;
	//}

	//mAnimator->Init(scene);


	////Final Check
	//if (currVertex == 0)
	//{
	//	throw Exception("AnimatedMesh::LoadModel(), Nothing was loaded from the Mesh File!");
	//}
	//
	//////Buffer Creation
	//mModelVertexBuffer.CreateBuffer(currVertex, sizeof(AnimationVertex), &mVertices[0]);
	//mModelIndexBuffer.CreateBuffer(currIndex, &mIndices[0]);
	//
	//ofstream vertexBinaryInfo;
	//vertexBinaryInfo.open("modelVertexInfo.bin", ios::out | ios::binary);
	//Save(vertexBinaryInfo);

	////Release Assimp ScenePtr
	//aiReleaseImport(scene);
}

void AnimatedModel::SetAnimator(SceneAnimator* animator)
{
	mAnimator.reset(animator);
}

void AnimatedModel::SetFilename(string filename)
{
	mFilename = filename;
}

void AnimatedModel::SetAnimation(string animation)
{
	mAnimator->SetAnimation(animation);
}

void AnimatedModel::SetAnimation(int index)
{
	mAnimator->SetAnimIndex(index);
}

void AnimatedModel::Update(float elapsedTime, CXMMATRIX ObjectWorld)
{
	mElapsedTime += elapsedTime;

	DrawMe(elapsedTime, ObjectWorld);
}

vector<XMFLOAT4X4> AnimatedModel::GetFinalTransforms()
{
	return mAnimator->GetTransforms(mElapsedTime);
}

void AnimatedModel::SetElapsedTime(float elapsedTime)
{
	mElapsedTime = elapsedTime;
}

vector<AnimationVertex>& AnimatedModel::GetVertices()
{
	return mVertices;
}

vector<uint16_t>& AnimatedModel::GetIndicies()
{
	return mIndices;
}

void AnimatedModel::BuildAnimatedModelPipelineParams()
{
	states = make_unique<CommonStates>(mDirect3DDevice);

	//Shaders
	mDirect3DDevice->CreateVertexShader(AnimatedVertexShader, sizeof(AnimatedVertexShader), nullptr, &mAnimVertexShader);
	mDirect3DDevice->CreateVertexShader(AnimatedVertexShader, sizeof(AnimatedVertexShader), nullptr, &mAnimVertexShader);
	mDirect3DDevice->CreatePixelShader(AnimatedPixelShader, sizeof(AnimatedPixelShader), nullptr, &mAnimPixelShader);

	//InputLayout
	D3D11_INPUT_ELEMENT_DESC inputElementDescriptions[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	mDirect3DDevice->CreateInputLayout(inputElementDescriptions, ARRAYSIZE(inputElementDescriptions), AnimatedVertexShader, sizeof(AnimatedVertexShader), &mAnimatedMeshInputLayout);

	//Pixel Shader Sampler
	mAnisoSamplerState = states->AnisotropicClamp();
	mDirect3DDeviceContext->PSSetSamplers(0, 1, &mAnisoSamplerState);
}

void AnimatedModel::Save(ofstream& fout)
{
	fout << "-Mesh-\r\n";
	fout << "#NumVertices " << mVertices.size() << "\r\n";
	fout << "#NumIndices " << mIndices.size() << "\r\n";
	fout << "#Vertices" << "\r\n";

	for (int i = 0; i < mVertices.size(); i++)
	{
		fout << "Pos: " << mVertices[i].Position.x << " " << mVertices[i].Position.y << " " << mVertices[i].Position.z << "\r\n";
		fout << "UV: " << mVertices[i].TextureCoordinates.x << " " << mVertices[i].TextureCoordinates.y << "\r\n";
		fout << "Normal: " << mVertices[i].Normal.x << " " << mVertices[i].Normal.y << " " << mVertices[i].Normal.z << "\r\n";
		fout << "Tangent: " << mVertices[i].Tangent.x << " " << mVertices[i].Tangent.y << " " << mVertices[i].Tangent.z << "\r\n";
		fout << "BoneId " << (int)mVertices[i].BoneIndices.x << " " << (int)mVertices[i].BoneIndices.y << " " << (int)mVertices[i].BoneIndices.z << " " << (int)mVertices[i].BoneIndices.w << "\r\n";
		fout << "Weights: " << mVertices[i].BoneWeights.x << " " << mVertices[i].BoneWeights.y << " " << mVertices[i].BoneWeights.z << " " << mVertices[i].BoneWeights.w << "\r\n";
		fout << "-\r\n";
	}

	fout << "\r\n#Indices" << "\r\n";

	for (int i = 0; i < mIndices.size(); i++)
		fout << mIndices[i] << " ";

	fout << "\r\n";


	ofstream skeletonBinaryInfo;
	skeletonBinaryInfo.open("modelSkeletonInfo.bin", ios::out | ios::binary);
	mAnimator->Save(skeletonBinaryInfo);
}

void AnimatedModel::Load(ifstream& fin)
{
	string ignore, texture;
	int numVertices, numIndices;
	fin >> ignore; // -Mesh-
	fin >> ignore >> numVertices; // #NumVertices
	fin >> ignore >> numIndices; // #NumIndices
	fin >> ignore; // #Vertices

				   // Read all vertices;
	vector<AnimationVertex> vertices;
	for (int i = 0; i < numVertices; i++)
	{
		AnimationVertex vertex;

		fin >> ignore >> vertex.Position.x >> vertex.Position.y >> vertex.Position.z;
		fin >> ignore >> vertex.TextureCoordinates.x >> vertex.TextureCoordinates.y;
		fin >> ignore >> vertex.Normal.x >> vertex.Normal.y >> vertex.Normal.z;
		fin >> ignore >> vertex.Tangent.x >> vertex.Tangent.y >> vertex.Tangent.z;
		fin >> ignore >> vertex.BoneIndices.x >> vertex.BoneIndices.y >> vertex.BoneIndices.z >> vertex.BoneIndices.w;
		fin >> ignore >> vertex.BoneWeights.x >> vertex.BoneWeights.y >> vertex.BoneWeights.z >> vertex.BoneWeights.w;
		fin >> ignore;

		vertices.push_back(vertex);
	}

	// Read all indices.
	fin >> ignore; // #Indices
	vector<uint16_t> indices;
	for (int i = 0; i < numIndices; i++)
	{
		uint16_t index;
		fin >> index;
		indices.push_back(index);
	}

	//Buffer Creation
	mModelVertexBuffer.CreateBuffer(numVertices, sizeof(AnimationVertex), &vertices[0]);
	mModelIndexBuffer.CreateBuffer(numIndices, &indices[0]);
	mModelNumVertices = numVertices;
	mModelNumIndices = numIndices;

	//Animator Population
	ifstream myfile("modelSkeletonInfo.bin", ios::binary);
	mAnimator->Load(myfile);
	////ifstream myVertexfile("modelVertexAndAnim.bin", ios::binary);
	////Load(myVertexfile);

	////Load Animation Data ANIMATION START
	////ifstream myfile("example.bin", ios::binary);
	////mAnimator->Load(myfile);

}

void AnimatedModel::UpdateConstantBuffers(float elapsedTime, CXMMATRIX ObjectWorld)
{
	//-Constant Buffer Updates-//
	cbPerObjectMatrix perObjCB0;
	XMVECTOR detVec;
	XMStoreFloat4x4(&perObjCB0.mWorldMatrix, XMMatrixTranspose(ObjectWorld));
	XMStoreFloat4x4(&perObjCB0.mViewProjMatrix, XMMatrixTranspose(ThirdPersonCamera::GetInstance()->GetViewProj()));
	XMStoreFloat4x4(&perObjCB0.mInvTransWorldMatrix, XMMatrixInverse(&detVec, XMMatrixTranspose(ObjectWorld)));
	mAnimatedModelMatrixInfo.Data = perObjCB0;
	mAnimatedModelMatrixInfo.ApplyChanges(mDirect3DDeviceContext);
	auto CB0 = mAnimatedModelMatrixInfo.Buffer();
	mDirect3DDeviceContext->VSSetConstantBuffers(CB_PER_OBJECT_MATRIX_REGISTER_SLOT, 1, &CB0);
	mDirect3DDeviceContext->PSSetConstantBuffers(CB_PER_OBJECT_MATRIX_REGISTER_SLOT, 1, &CB0);

	//Material Scalars
	mAnimatedModelMaterialInfo.Data = GetMaterial();;
	mAnimatedModelMaterialInfo.ApplyChanges(mDirect3DDeviceContext);
	auto CB1 = mAnimatedModelMaterialInfo.Buffer();
	mDirect3DDeviceContext->VSSetConstantBuffers(CB_PER_OBJECT_MATERIAL_REGISTER_SLOT, 1, &CB1);
	mDirect3DDeviceContext->PSSetConstantBuffers(CB_PER_OBJECT_MATERIAL_REGISTER_SLOT, 1, &CB1);

	//Bone Transforms
	cbPerObjectSkinningData perObjCB2;
	copy(mAnimator->GetTransforms(elapsedTime).begin(), mAnimator->GetTransforms(elapsedTime).end(), perObjCB2.BoneTransforms);
	for (size_t i = 0; i < ARRAYSIZE(perObjCB2.BoneTransforms); i++)
	{
		XMStoreFloat4x4(&perObjCB2.BoneTransforms[i], XMMatrixTranspose(XMLoadFloat4x4(&perObjCB2.BoneTransforms[i])));
	}
	mCalculatedBoneTransforms.Data = perObjCB2;
	mCalculatedBoneTransforms.ApplyChanges(mDirect3DDeviceContext);
	auto CB2 = mCalculatedBoneTransforms.Buffer();
	mDirect3DDeviceContext->VSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &CB2);
	mDirect3DDeviceContext->PSSetConstantBuffers(CB_PER_OBJECT_SKINNING_REGISTER_SLOT, 1, &CB2);
}

void AnimatedModel::DrawMe(float elapsedTime, CXMMATRIX ObjectWorld)
{
	mDirect3DDeviceContext->OMSetDepthStencilState(states->DepthDefault(), 0);
	mDirect3DDeviceContext->RSSetState(states->CullCounterClockwise());
	mDirect3DDeviceContext->IASetPrimitiveTopology(GetModelTopology());
	float mOBlendFactor[4] = { NULL, NULL, NULL, NULL };
	UINT mSampleMask = 0xFFFFFFFF;
	//mDirect3DDeviceContext->OMSetBlendState(states->Additive(), mOBlendFactor, mSampleMask);
	mDirect3DDeviceContext->IASetInputLayout(mAnimatedMeshInputLayout);
	mDirect3DDeviceContext->VSSetShader(mAnimVertexShader, nullptr, 0);
	mDirect3DDeviceContext->PSSetShader(mAnimPixelShader, nullptr, 0);

	mModelVertexBuffer.BindBuffer();
	mModelIndexBuffer.BindBuffer();

	UpdateConstantBuffers(elapsedTime, ObjectWorld);

	mDirect3DDeviceContext->PSSetSamplers(0, 1, &mAnisoSamplerState);
	mDirect3DDeviceContext->PSSetShaderResources(0, 1, &mModelDiffuseTexture);
	mDirect3DDeviceContext->PSSetShaderResources(1, 1, &mModelNormalMapTexture);

	mDirect3DDeviceContext->DrawIndexed(mModelNumIndices, mModelStartingIndex, 0);
}


string AnimatedModel::GetFilename()
{
	return mFilename;
}

cbPerObjectMaterial AnimatedModel::GetMaterial()
{
	mMaterialInfo.mDiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterialInfo.mSpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterialInfo.mAmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterialInfo.mEmissiveColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mMaterialInfo.mSpecularPower = 10.0f;
	return mMaterialInfo;
}

void AnimatedModel::LoadMaterialScalars(AnimatedModel* aModel, const aiMaterial* meshMaterial)
{
	aModel->mMaterialInfo.mDiffuseColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor4D*>(&aModel->mMaterialInfo.mDiffuseColor));
	aModel->mMaterialInfo.mSpecularColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor4D*>(&aModel->mMaterialInfo.mSpecularColor));
	aModel->mMaterialInfo.mAmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_AMBIENT, reinterpret_cast<aiColor4D*>(&aModel->mMaterialInfo.mAmbientColor));
	aModel->mMaterialInfo.mEmissiveColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	aiGetMaterialColor(meshMaterial, AI_MATKEY_COLOR_EMISSIVE, reinterpret_cast<aiColor4D*>(&aModel->mMaterialInfo.mEmissiveColor));
	aModel->mMaterialInfo.mSpecularPower = 10.0f;
	aiGetMaterialFloat(meshMaterial, AI_MATKEY_SHININESS_STRENGTH, &aModel->mMaterialInfo.mSpecularPower);
}

void AnimatedModel::LoadTextures(AnimatedModel* aModel)
{
	//Diffuse Texture
	wostringstream diffuseTextureFullPath;
	string diffText = string("character_D.png");
	diffuseTextureFullPath << diffText.c_str();
	CreateWICTextureFromFile(mDirect3DDevice, mDirect3DDeviceContext, diffuseTextureFullPath.str().c_str(), nullptr, &aModel->mModelDiffuseTexture);

	//Normal Map
	wostringstream normalTextureFullPath;
	string normText = string("character_N.png");
	normalTextureFullPath << normText.c_str();
	CreateWICTextureFromFile(mDirect3DDevice, mDirect3DDeviceContext, normalTextureFullPath.str().c_str(), nullptr, &aModel->mModelNormalMapTexture);
}

int AnimatedModel::GetCurrentAnimation()
{
	return mAnimator->GetAnimationIndex();
}

void AnimatedModel::AdjustAnimationSpeedBy(float percent)
{
	mAnimator->AdjustAnimationSpeedBy(percent);
}

float AnimatedModel::GetAnimationSpeed()
{
	return mAnimator->GetAnimationSpeed();
}

size_t AnimatedModel::GetNumAnimations()
{
	return mAnimator->Animations.size();
}

VertexBufferWrapper<AnimationVertex>& AnimatedModel::GetVertexBuffer()
{
	return mModelVertexBuffer;
}

IndexBufferWrapper& AnimatedModel::GetIndexBuffer()
{
	return mModelIndexBuffer;
}