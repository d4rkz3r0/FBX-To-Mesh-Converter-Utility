#include "FBXConverter.h"
#include <fstream>

void AddBone(FbxNode* _pNode, int32_t _nodeIndex, int32_t _parentIndex, animated_skeleton& _skeleton)
{
	if (_pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		_skeleton.bones.resize(_nodeIndex + 1);

		animated_bone bone;
		bone.parentIndex = _parentIndex;
		bone.name = _pNode->GetName();
		bone.numKeyFrames = 0;
		bone.hasAnimationData = false;
		bone.localMatrix = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		bone.bindPoseInverse = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

		_skeleton.bones[_nodeIndex] = bone;
	}

	for (int childIterator = 0; childIterator < (int32_t)_pNode->GetChildCount(); childIterator++)
	{
		AddBone(_pNode->GetChild(childIterator), (int32_t)_skeleton.bones.size(), _nodeIndex, _skeleton);
	}
}

int FindBoneByName(animated_skeleton& _skeleton, string _name)
{
	int boneIndex = -1;
	for (int i = 0; i < _skeleton.bones.size(); i++)
	{
		if (strcmp(_skeleton.bones[i].name.c_str(), _name.c_str()) == 0)
		{
			boneIndex = i;
			return boneIndex;
		}
	}

	return  boneIndex;
}

DirectX::XMFLOAT4X4 MajorFlip(FbxMatrix _input)
{
	DirectX::XMFLOAT4X4 output;
	_input = _input.Transpose();

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			output.m[i][j] = (float)_input.Get(j, i);

	return output;
}

void ConnectParents(FbxNode* currJoint, FbxNode* prevJoint, animated_skeleton _skeleton)
{

	string currentName;
	int parent = -1, child = -1;
	for (int j = 0; j < _skeleton.bones.size(); j++)
	{
		currentName = currJoint->GetName();
		if (currentName == _skeleton.bones[j].name)
			child = j;
		if (prevJoint != nullptr && prevJoint->GetName() == _skeleton.bones[j].name)
			parent = j;
	}
	if (child != -1)
	{
		_skeleton.bones[child].parentIndex = parent;
		int childCount = currJoint->GetChildCount();
		for (int i = 0; i < childCount; i++)
		{
			ConnectParents(currJoint->GetChild(i), currJoint, _skeleton);
		}
		return;
	}
	return;
}

void NormalizeWeights(DirectX::XMFLOAT4& weights)
{
	float sum = .000000000000001f;

	sum = weights.x + weights.y + weights.z + weights.w;

	weights.x /= sum; weights.y /= sum; weights.z /= sum; weights.w /= sum;

	weights.w = 1.0f - weights.x - weights.y - weights.z;
}

FBXConverter::FBXConverter()
{
}


FBXConverter::~FBXConverter()
{
}

unsigned int FBXConverter::Convert(char * _filename)
{
	return Read(_filename);
}

unsigned int FBXConverter::Read(char * _filename)
{
	FbxManager* manager = nullptr;
	manager = FbxManager::Create();

	FbxIOSettings* ioSettings = FbxIOSettings::Create(manager, IOSN_IMPORT);

	ioSettings->SetBoolProp(IMP_FBX_TEXTURE, true);
	ioSettings->SetBoolProp(IMP_FBX_MODEL, true);
	ioSettings->SetBoolProp(IMP_FBX_ANIMATION, true);

	manager->SetIOSettings(ioSettings);

	FbxImporter* importer = FbxImporter::Create(manager, "");
	FbxScene* scene = FbxScene::Create(manager, "My Scene");

	importer->Initialize(_filename, -1, manager->GetIOSettings());
	importer->Import(scene);
	importer->Destroy();

	vector<FbxMesh*> vpFbxMesh;

	/*
	Starting at the root node, find all meshes in the scene and add them to the vector of FbxMesh pointers.
	*/
	FbxNode* pRootNode = scene->GetRootNode();
	if (pRootNode)
	{
		int childCount = pRootNode->GetChildCount();
		for (int i = 0; i < childCount; i++)
		{
			FbxNode* pChild = pRootNode->GetChild(i);
			if (pChild)
			{
				if (pChild->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eMesh)
				{
					FbxMesh* pMesh = (FbxMesh*)pChild->GetNodeAttribute();
					vpFbxMesh.push_back(pMesh);
				}
			}
		}
	}
	else return 0;

	for each (FbxMesh* pMesh in vpFbxMesh)
	{
		string meshName = pMesh->GetName();

		vector<animated_vert> vVertex;
		animated_skeleton skeleton;
		ExtractModel(pMesh, vVertex);
		ExtractSkeleton(pMesh, scene, skeleton);

		animated_animation animation;
		ExtractAnimations(pMesh, scene, animation, vVertex, skeleton);

		Write(_filename, animation, vVertex, skeleton);

		pMesh->Destroy();
	}


	pRootNode->Destroy();
	scene->Destroy();
	ioSettings->Destroy();
	manager->Destroy();

	return 1;
}

unsigned int FBXConverter::Write(char * _filename, animated_animation& _animation, vector<animated_vert>& _vVertex, animated_skeleton& _skeleton)
{
	// For now, I'm going to write the mesh and skeleton separately.
	// This is so that we only load the animated skeletons for each animation in engine, and not the entire model again.
	// The model will only need loaded once, and each animation/skeleton loaded on a per animation basis.

	ofstream binaryWriter;

	char filename[128];
	int namelength = (int)strlen(_filename);

	strcpy_s(filename, _filename);

	/// The Model
	filename[namelength - 3] = 'f';
	filename[namelength - 2] = 'b';
	filename[namelength - 1] = 'i';

	binaryWriter.open(filename, ios_base::binary | ios_base::trunc);

	if (binaryWriter.is_open())
	{
		uint32_t vertexCount = (uint32_t)_vVertex.size();

		binaryWriter.write(reinterpret_cast<char*>(&vertexCount), sizeof(uint32_t));

		for each (animated_vert data in _vVertex)
		{
			binaryWriter.write(reinterpret_cast<char*>(&data), sizeof(animated_vert));
		}

		binaryWriter.close();
	}

	strcpy_s(filename, _filename);

	/// The Skeleton
	int currentLocation, periodLocation;
	currentLocation = periodLocation = namelength - 4;

	for (int j = 0; currentLocation < periodLocation + _animation.name.length(); currentLocation++, j++)
	{
		filename[currentLocation] = _animation.name[j];
	}

	filename[currentLocation] = '.';
	filename[currentLocation + 1] = 'f';
	filename[currentLocation + 2] = 'b';
	filename[currentLocation + 3] = 'a';
	filename[currentLocation + 4] = '\0';

	binaryWriter.open(filename, ios_base::binary | ios_base::trunc);

	if (binaryWriter.is_open())
	{
		uint32_t boneCount = (uint32_t)_skeleton.bones.size();

		binaryWriter.write(reinterpret_cast<char*>(&boneCount), sizeof(uint32_t));

		for each (animated_bone data in _skeleton.bones)
		{
			uint32_t stringLength = (uint32_t)data.name.size();
			binaryWriter.write(reinterpret_cast<char*>(&stringLength), sizeof(uint32_t));
			binaryWriter.write(data.name.c_str(), data.name.length());
			binaryWriter.write(reinterpret_cast<char*>(&data.hasAnimationData), sizeof(bool));
			binaryWriter.write(reinterpret_cast<char*>(&data.parentIndex), sizeof(int32_t));
			binaryWriter.write(reinterpret_cast<char*>(&data.numKeyFrames), sizeof(uint32_t));

			for each (animated_keyframe frame in data.vKeyframe)
			{
				binaryWriter.write(reinterpret_cast<char*>(&frame), sizeof(animated_keyframe));
			}

			binaryWriter.write(reinterpret_cast<char*>(&data.bindPoseInverse), sizeof(DirectX::XMFLOAT4X4));
			binaryWriter.write(reinterpret_cast<char*>(&data.localMatrix), sizeof(DirectX::XMFLOAT4X4));
		}

		binaryWriter.close();

		//struct animated_bone
		//{
		//	string name;
		//
		//	bool hasAnimationData;
		//	int32_t parentIndex;
		//	uint32_t numKeyFrames;
		//
		//	vector<animated_keyframe> vKeyframe;
		//
		//	DirectX::XMFLOAT4X4 bindPoseInverse;
		//	DirectX::XMFLOAT4X4 localMatrix;
		//};
	}

	return 0;
}

void FBXConverter::ExtractModel(FbxMesh* _pMesh, vector<animated_vert>& _vVertex)
{
	vector<DirectX::XMFLOAT4> vControlPoints;
	ExtractControlPoints(_pMesh, vControlPoints);
	ExtractVertexInformation(_pMesh, _vVertex, vControlPoints);
}

void FBXConverter::ExtractControlPoints(FbxMesh* _pMesh, vector<DirectX::XMFLOAT4>& _vControlPoints)
{
	// A control point is a synonym for 'vertex' but only contains the (x,y,z) position.
	int numControlPoints = _pMesh->GetControlPointsCount();

	// A control point is generally unique, as multiple 'polygon vertex's can reference the same control point.
	_vControlPoints.resize(numControlPoints);
	for (int i = 0; i < numControlPoints; i++)
	{
		_vControlPoints[i] = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		_vControlPoints[i].x = static_cast<float>(_pMesh->GetControlPointAt(i).mData[0]);
		_vControlPoints[i].y = static_cast<float>(_pMesh->GetControlPointAt(i).mData[1]);
		_vControlPoints[i].z = static_cast<float>(_pMesh->GetControlPointAt(i).mData[2]);
	}
}

void FBXConverter::ExtractVertexInformation(FbxMesh * _pMesh, vector<animated_vert>& _vVertex, vector<DirectX::XMFLOAT4>& _vControlPoints)
{
	int triangleCount = _pMesh->GetPolygonCount();
	int uniqueVertCount = 0;

	_vVertex.resize(triangleCount * 3);

	for (int triangleIteration = 0; triangleIteration < triangleCount; triangleIteration++)
	{
		for (int pointIteration = 0; pointIteration < 3; pointIteration++)
		{
			animated_vert tempAnimatedVert;

			// Get position from the control points generated in ExtractControlPoints
			int controlPointIndex = _pMesh->GetPolygonVertex(triangleIteration, pointIteration);
			tempAnimatedVert.pos = _vControlPoints[controlPointIndex];

			// Get UV from the FbxMesh
			FbxVector2 uvValue = FbxVector2(0.0f, 0.0f);
			FbxLayerElementUV* uvElement = _pMesh->GetLayer(0)->GetUVs();
			if (uvElement)
			{
				int uvAccessIndex = 0;
				switch (uvElement->GetMappingMode())
				{
				case FbxLayerElement::eByControlPoint:
				{
					uvAccessIndex = controlPointIndex;
					break;
				}
				case FbxLayerElement::eByPolygonVertex:
				{
					uvAccessIndex = _pMesh->GetTextureUVIndex(triangleIteration, pointIteration);
					break;
				}
				}

				uvValue = uvElement->GetDirectArray().GetAt(uvAccessIndex);

				tempAnimatedVert.uv.x = static_cast<float>(uvValue.mData[0]);
				tempAnimatedVert.uv.y = static_cast<float>(uvValue.mData[1]);
			}

			// Get Normal from the FbxMesh
			FbxVector4 normalValue = FbxVector4(0.0, 0.0, 0.0, 0.0);
			_pMesh->GetPolygonVertexNormal(triangleIteration, pointIteration, normalValue);

			tempAnimatedVert.normal.x = static_cast<float>(normalValue.mData[0]);
			tempAnimatedVert.normal.y = static_cast<float>(normalValue.mData[1]);
			tempAnimatedVert.normal.z = static_cast<float>(normalValue.mData[2]);
			tempAnimatedVert.normal.w = static_cast<float>(normalValue.mData[3]);

			_vVertex[uniqueVertCount] = tempAnimatedVert;
			uniqueVertCount++;
		}
	}
}

void FBXConverter::ExtractSkeleton(FbxMesh * _pMesh, FbxScene * _pScene, animated_skeleton & _skeleton)
{
	FbxNode* rootNode = _pScene->GetRootNode();
	for (int childIterator = 0; childIterator < rootNode->GetChildCount(); childIterator++)
	{
		FbxNode* childNode = rootNode->GetChild(childIterator);
		AddBone(childNode, 0, -1, _skeleton);
	}
}

void FBXConverter::ExtractAnimations(FbxMesh * _pMesh, FbxScene* _pScene, animated_animation& _animation, vector<animated_vert>& _vVertex, animated_skeleton & _skeleton)
{
	int rootIndex = -1;

	int deformerCount = _pMesh->GetDeformerCount();
	for (int deformerIterator = 0; deformerIterator < deformerCount; deformerIterator++)
	{
		FbxSkin* currentSkin = nullptr;

		currentSkin = static_cast<FbxSkin*>(_pMesh->GetDeformer(deformerIterator, FbxDeformer::eSkin));

		if (currentSkin)
		{
			int clusterCount = currentSkin->GetClusterCount();
			for (int clusterIterator = 0; clusterIterator < clusterCount; clusterIterator++)
			{
				FbxCluster* currentCluster = currentSkin->GetCluster(clusterIterator);

				string parentName = currentCluster->GetLink()->GetParent()->GetName();
				if (strcmp(parentName.c_str(), "RootNode") == 0)
					rootIndex = clusterIterator;

				// The link is connected to a bone.
				string linkName = currentCluster->GetLink()->GetName();

				int currentBoneIndex = FindBoneByName(_skeleton, linkName);

				DirectX::XMFLOAT4X4 transformMatrix, transformLink, bindPoseInverse;
				FbxAMatrix fbxTransformMatrix, fbxTransformLink, fbxBindPoseInverse;

				currentCluster->GetTransformMatrix(fbxTransformMatrix);
				currentCluster->GetTransformLinkMatrix(fbxTransformLink);

				fbxBindPoseInverse =
					fbxTransformLink.Inverse() * fbxTransformMatrix *
					FbxAMatrix(_pMesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot), _pMesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot), _pMesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot));

				transformMatrix = MajorFlip(fbxTransformMatrix);
				transformLink = MajorFlip(fbxTransformLink);
				bindPoseInverse = MajorFlip(fbxBindPoseInverse);

				_skeleton.bones[currentBoneIndex].bindPoseInverse = bindPoseInverse;
				_skeleton.bones[currentBoneIndex].localMatrix = MajorFlip(currentCluster->GetLink()->EvaluateLocalTransform());

				FbxAnimStack* currentAnimationStack = _pScene->GetSrcObject<FbxAnimStack>(0);
				FbxTakeInfo* currentTakeInfo = _pScene->GetTakeInfo(currentAnimationStack->GetName());
				FbxTime startTime = currentTakeInfo->mLocalTimeSpan.GetStart();
				FbxTime endTime = currentTakeInfo->mLocalTimeSpan.GetStop();

				_animation.name = currentAnimationStack->GetName();
				_animation.duration = (endTime.GetMilliSeconds() - startTime.GetMilliSeconds()) / 1000.0f;

				for (FbxLongLong time = startTime.GetFrameCount(FbxTime::eFrames24); time <= endTime.GetFrameCount(FbxTime::eFrames24); time++)
				{
					_skeleton.bones[currentBoneIndex].hasAnimationData = true;
					_skeleton.bones[currentBoneIndex].numKeyFrames++;

					FbxTime currentTime;
					currentTime.SetFrame(time, FbxTime::eFrames24);

					FbxAMatrix transformOffset = _pMesh->GetNode()->EvaluateLocalTransform(currentTime)
						* FbxAMatrix(_pMesh->GetNode()->GetGeometricTranslation(FbxNode::eSourcePivot), _pMesh->GetNode()->GetGeometricRotation(FbxNode::eSourcePivot), _pMesh->GetNode()->GetGeometricScaling(FbxNode::eSourcePivot));

					animated_keyframe keyframeInfo;
					keyframeInfo.currentTime = static_cast<float>(currentTime.GetMilliSeconds() / 1000.0f);
					keyframeInfo.frameNumber = (uint32_t)time;
					keyframeInfo.globalTransform = MajorFlip(transformOffset.Inverse() * currentCluster->GetLink()->EvaluateLocalTransform(currentTime));

					_skeleton.bones[currentBoneIndex].vKeyframe.push_back(keyframeInfo);
					_skeleton.bones[currentBoneIndex].numKeyFrames = (uint32_t)_skeleton.bones[currentBoneIndex].vKeyframe.size();
				}
			}

			// Commented out because Culling non-anmiated joints causes parenting to be messed up.
			//int iterCount = 0;
			//for (auto itr = _skeleton.bones.begin(); itr != _skeleton.bones.end(); itr++)
			//{
			//	if (itr->numKeyFrames == 0)
			//	{
			//		_skeleton.bones.erase(itr);
			//		itr = _skeleton.bones.begin();
			//		itr += iterCount;
			//	}
			//	else
			//		iterCount++;
			//}

			ConnectParents(currentSkin->GetCluster(rootIndex)->GetLink(), nullptr, _skeleton);

			// Give the indices their bones and weights.
			for (int deformerIterator = 0; deformerIterator < deformerCount; deformerIterator++)
			{
				FbxSkin* currentSkin = static_cast<FbxSkin*>(_pMesh->GetDeformer(deformerIterator, FbxDeformer::eSkin));

				if (currentSkin)
				{
					int clusterCount = currentSkin->GetClusterCount();
					for (int clusterIterator = 0; clusterIterator < clusterCount; clusterIterator++)
					{
						FbxCluster* currentCluster = currentSkin->GetCluster(clusterIterator);

						string linkName = currentCluster->GetLink()->GetName();
						int currentBoneIndex = FindBoneByName(_skeleton, linkName);

						float* weightArray = reinterpret_cast<float*>(currentCluster->GetControlPointWeights());
						int* indexArray = currentCluster->GetControlPointIndices();

						int indexCount = currentCluster->GetControlPointIndicesCount();  // The number of control points moved by this cluster.

						DirectX::XMFLOAT4X4 indices, weights;
						for (int indexIterator = 0; indexIterator < indexCount; indexIterator++)
						{
							int	boneIndex = currentBoneIndex;
							float weight = (float)currentCluster->GetControlPointWeights()[indexIterator];

							for (int boneCheck = 0; boneCheck < 4; boneCheck++)
							{
								if (_vVertex[indexArray[indexIterator]].bones[boneCheck] == -1)
								{
									_vVertex[indexArray[indexIterator]].bones[boneCheck] = boneIndex;
									switch (boneCheck)
									{
									case 0:
									{
										_vVertex[indexArray[indexIterator]].weights.x = weight;
										break;
									}
									case 1:
									{
										_vVertex[indexArray[indexIterator]].weights.y = weight;
										break;
									}
									case 2:
									{
										_vVertex[indexArray[indexIterator]].weights.z = weight;
										break;
									}
									case 3:
									{
										_vVertex[indexArray[indexIterator]].weights.w = weight;
										break;
									}
									}
									break;
								}
							}
						}
					}
				}
			}

			int boneIndex = 0;
			float weight = 0.0f;

			for (int vertexIterator = 0; vertexIterator < _vVertex.size(); vertexIterator++)
			{
				NormalizeWeights(_vVertex[vertexIterator].weights);
			}
		}
	}
}

