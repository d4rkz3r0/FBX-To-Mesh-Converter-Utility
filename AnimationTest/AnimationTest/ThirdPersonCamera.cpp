#include "ThirdPersonCamera.h"
#include "InputManager.h"
#include "CommonIncludes.h"

bool ThirdPersonCamera::mFreeLookCamera = false;

ThirdPersonCamera::ThirdPersonCamera()
{
	if(!mFreeLookCamera)
	{
		//Initialize Camera Transform Information
		mOffsetDistance = 0.0f;
		mHeadingDegrees = 0.0f;
		mPitchDegrees = 0.0f;


		SetPosition(0.0f, 0.0f, 0.0f);
		SetTarget(0.0f, 0.0f, 0.0f);

		SetRight(1.0f, 0.0f, 0.0f);
		SetUp(0.0f, 1.0f, 0.0f);
		SetLook(0.0f, 0.0f, 1.0f);

		mVelocity = XMFLOAT3(0.0f, 0.0f, 0.0f);

		XMStoreFloat4x4(&mView, XMMatrixIdentity());
		XMStoreFloat4x4(&mProj, XMMatrixIdentity());
		SetOrientationXM(XMQuaternionIdentity());
	}
}

void ThirdPersonCamera::Pie()
{
	Update();
}

void ThirdPersonCamera::Initialize(float aspectRatio, float FoV, float NearZ, float FarZ, bool lockOnPlayer)
{
	//Projection Matrix Setup
	mFoV = FoV;
	mAspectRatio = aspectRatio;
	mNearZ = NearZ;
	mFarZ = FarZ;
	SetFrustum(mFoV, mAspectRatio, mNearZ, mFarZ);

	//Init
	//mPlayerGameObject.reset();

	if (!mFreeLookCamera)
	{
		SetPlayerPosition(0.0f, 1.0f + mPlayerRadius, 0.0f);
		XMFLOAT3 cameraInitialOffsettedPosition = XMFLOAT3
		(
			0.0f,
			50.0f,
			-mPlayerRadius * mCameraZOffsetScalar
		);

		XMFLOAT3 cameraInitialLookAtTarget = XMFLOAT3(0.0f, 0.0f, 0.0f);
		XMFLOAT3 cameraInitialUpVector = XMFLOAT3(0.0f, 1.0f, 0.0f);

		LookAt(cameraInitialOffsettedPosition, cameraInitialLookAtTarget, cameraInitialUpVector);
	}
}

void ThirdPersonCamera::Update()
{

	FPSCamKeyboardInput();
	UpdateFPSViewMatrix();
}

void ThirdPersonCamera::Shutdown()
{
	//mPlayerGameObject.release();
	DestroyInstance();
}

XMFLOAT3 ThirdPersonCamera::GetRight()
{
	return mRight;
}

XMVECTOR ThirdPersonCamera::GetRightXM()
{
	return XMLoadFloat3(&mRight);
}

XMFLOAT3 ThirdPersonCamera::GetUp()
{
	return mUp;
}

XMVECTOR ThirdPersonCamera::GetUpXM()
{
	return XMLoadFloat3(&mUp);
}

XMFLOAT3 ThirdPersonCamera::GetLook()
{
	return mLook;
}

XMVECTOR ThirdPersonCamera::GetLookXM()
{
	return XMLoadFloat3(&mLook);
}

XMFLOAT3 ThirdPersonCamera::GetPosition()
{
	return mPosition;
}

XMFLOAT3 ThirdPersonCamera::GetPlayerPosition()
{
	return mPlayerPosition;
}

XMFLOAT4 ThirdPersonCamera::GetOrientation()
{
	return mOrientation;
}

XMFLOAT3 ThirdPersonCamera::GetCameraTarget()
{
	return mTarget;
}

XMVECTOR ThirdPersonCamera::GetPositionXM()
{
	return XMLoadFloat3(&mPosition);
}

XMVECTOR ThirdPersonCamera::GetOrientationXM()
{
	return XMLoadFloat4(&mOrientation);
}

XMVECTOR ThirdPersonCamera::GetTargetXM()
{
	return XMLoadFloat3(&mTarget);
}

XMVECTOR ThirdPersonCamera::GetTargetYAxisXM()
{
	return XMLoadFloat3(&mTargetYAxis);
}

XMMATRIX ThirdPersonCamera::GetView()
{
	return XMLoadFloat4x4(&mView);
}

XMMATRIX ThirdPersonCamera::GetProj()
{
	return XMLoadFloat4x4(&mProj);
}

XMMATRIX ThirdPersonCamera::GetViewProj()
{
	return (XMMatrixMultiply(GetView(), GetProj()));
}


void ThirdPersonCamera::RotateCamera(float headingDegrees, float pitchDegrees)
{
	mHeadingDegrees = -headingDegrees;
	mPitchDegrees = -pitchDegrees;
}

void ThirdPersonCamera::SetRight(float x, float y, float z)
{
	mRight = XMFLOAT3(x, y, z);
}

void ThirdPersonCamera::SetRight(const XMFLOAT3& right)
{
	mRight = right;
}

void ThirdPersonCamera::SetRightXM(const XMVECTOR& right)
{
	XMStoreFloat3(&mRight, right);
}

void ThirdPersonCamera::SetUp(float x, float y, float z)
{
	mUp = XMFLOAT3(x, y, z);
}

void ThirdPersonCamera::SetUp(const XMFLOAT3& up)
{
	mUp = up;
}

void ThirdPersonCamera::SetUpXM(const XMVECTOR& up)
{
	XMStoreFloat3(&mUp, up);
}

void ThirdPersonCamera::SetLook(float x, float y, float z)
{
	mLook = XMFLOAT3(x, y, z);
}

void ThirdPersonCamera::SetLook(const XMFLOAT3& look)
{
	mLook = look;
}

void ThirdPersonCamera::SetLookXM(const XMVECTOR& look)
{
	XMStoreFloat3(&mLook, look);
}

void ThirdPersonCamera::SetPosition(float x, float y, float z)
{
	mPosition = XMFLOAT3(x, y, z);
}

void ThirdPersonCamera::SetPosition(const XMFLOAT3& position)
{
	mPosition = position;
}

void ThirdPersonCamera::SetPlayerPosition(float x, float y, float z)
{
	mPlayerPosition = XMFLOAT3(x, y, z);
}

void ThirdPersonCamera::SetPlayerPosition(const XMFLOAT3& position)
{
	mPlayerPosition = position;
}

void ThirdPersonCamera::SetTarget(const XMFLOAT3& target)
{
	mTarget = target;
}

void ThirdPersonCamera::SetPositionXM(const XMVECTOR& position)
{
	XMStoreFloat3(&mPosition, position);
}

void ThirdPersonCamera::SetOrientation(float xComponent, float yComponent, float zComponent, float wComponent)
{
	mOrientation = XMFLOAT4(xComponent, yComponent, zComponent, wComponent);
}

void ThirdPersonCamera::SetOrientation(const XMFLOAT4& orientation)
{
	mOrientation = orientation;
}

void ThirdPersonCamera::SetOrientationXM(const XMVECTOR& orientation)
{
	XMStoreFloat4(&mOrientation, orientation);
}

void ThirdPersonCamera::SetTarget(float x, float y, float z)
{
	mTarget = XMFLOAT3(x, y, z);
}

void ThirdPersonCamera::SetTargetXM(const XMVECTOR& target)
{
	XMStoreFloat3(&mTarget, target);
}

void ThirdPersonCamera::SetTargetYAxis(const XMFLOAT3& targetYAxis)
{
	mTargetYAxis = targetYAxis;
}

void ThirdPersonCamera::SetTargetYAxisXM(const XMVECTOR& yAxis)
{
	XMStoreFloat3(&mTargetYAxis, yAxis);
}

void ThirdPersonCamera::SetOffsetDistance(float offsetDistance)
{
	mOffsetDistance = offsetDistance;
}

void ThirdPersonCamera::SetFrustum(float foVY, float aspectRatio, float nearZ, float farZ)
{
	mFoV = foVY;
	mAspectRatio = aspectRatio;
	mNearZ = nearZ;
	mFarZ = farZ;

	XMStoreFloat4x4(&mProj, XMMatrixPerspectiveFovLH(mFoV, mAspectRatio, mNearZ, mFarZ));
}

void ThirdPersonCamera::VFly(float moveSpeed)
{
	XMVECTOR vMoveSpeed = XMVectorReplicate(moveSpeed);
	XMVECTOR vGlobalY = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR vPosition = XMLoadFloat3(&mPosition);

	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vMoveSpeed, vGlobalY, vPosition));
}

void ThirdPersonCamera::Strafe(float moveSpeed)
{
	XMVECTOR vMoveSpeed = XMVectorReplicate(moveSpeed);
	XMVECTOR vPosition = XMLoadFloat3(&mPosition);
	XMVECTOR vRight = XMLoadFloat3(&mRight);

	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vMoveSpeed, vRight, vPosition));
}

void ThirdPersonCamera::Step(float moveSpeed)
{
	XMVECTOR vMoveSpeed = XMVectorReplicate(moveSpeed);
	XMVECTOR vPosition = XMLoadFloat3(&mPosition);
	XMVECTOR vLook = XMLoadFloat3(&mLook);

	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(vMoveSpeed, vLook, vPosition));
}

void ThirdPersonCamera::Pitch(float angle)
{
	XMMATRIX camXRotationMX = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	//Grab camera Y and Z axes
	XMVECTOR vUp = XMLoadFloat3(&mUp);
	XMVECTOR vLook = XMLoadFloat3(&mLook);

	//Don't use RightRotationMX's position vector when multiplying
	XMVECTOR vUpdatedUp = XMVector3TransformNormal(vUp, camXRotationMX);
	XMVECTOR vUpdatedLook = XMVector3TransformNormal(vLook, camXRotationMX);

	XMStoreFloat3(&mUp, vUpdatedUp);
	XMStoreFloat3(&mLook, vUpdatedLook);
}

void ThirdPersonCamera::Yaw(float angle)
{
	XMMATRIX globalYRotationMX = XMMatrixRotationY(angle);

	//Grab all camera axes
	XMVECTOR vRight = XMLoadFloat3(&mRight);
	XMVECTOR vLook = XMLoadFloat3(&mLook);
	XMVECTOR vUp = XMLoadFloat3(&mUp);

	XMVECTOR vUpdatedRight = XMVector3TransformNormal(vRight, globalYRotationMX);
	XMVECTOR vUpdatedUp = XMVector3TransformNormal(vUp, globalYRotationMX);
	XMVECTOR vUpdatedLook = XMVector3TransformNormal(vLook, globalYRotationMX);

	XMStoreFloat3(&mRight, vUpdatedRight);
	XMStoreFloat3(&mLook, vUpdatedLook);
	XMStoreFloat3(&mUp, vUpdatedUp);
}

void ThirdPersonCamera::UpdateFPSViewMatrix()
{
	//Load
	XMVECTOR vRight = XMLoadFloat3(&mRight);
	XMVECTOR vUp = XMLoadFloat3(&mUp);
	XMVECTOR vLook = XMLoadFloat3(&mLook);
	XMVECTOR vPosition = XMLoadFloat3(&mPosition);

	//(Camera's Orientation)
	//Re-Orthonormalize Basis Vectors
	vLook = XMVector3Normalize(vLook);
	vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));
	vRight = XMVector3Cross(vUp, vLook);

	//(Camera's Position)
	float newXPos = -XMVectorGetX(XMVector3Dot(vPosition, vRight));
	float newYPos = -XMVectorGetX(XMVector3Dot(vPosition, vUp));
	float newZPos = -XMVectorGetX(XMVector3Dot(vPosition, vLook));

	//Save
	XMStoreFloat3(&mRight, vRight);
	XMStoreFloat3(&mUp, vUp);
	XMStoreFloat3(&mLook, vLook);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = newXPos;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = newYPos;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = newZPos;

	mView(0, 3) = 0.0f;
	mView(1, 3) = 0.0f;
	mView(2, 3) = 0.0f;
	mView(3, 3) = 1.0f;
}

void ThirdPersonCamera::FPSCamKeyboardInput()
{
	if (InputManager::GetInstance()->GetKeyboard()->IsKeyDown(DIK_W))
	{
		Step(mCameraMovementRate * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
	}

	if (InputManager::GetInstance()->GetKeyboard()->IsKeyDown(DIK_S))
	{
		Step(-mCameraMovementRate  * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
	}

	if (InputManager::GetInstance()->GetKeyboard()->IsKeyDown(DIK_A))
	{
		Strafe(-mCameraMovementRate * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
	}

	if (InputManager::GetInstance()->GetKeyboard()->IsKeyDown(DIK_D))
	{
		Strafe(mCameraMovementRate * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
	}

	if (InputManager::GetInstance()->GetKeyboard()->IsKeyDown(DIK_Q) || InputManager::GetInstance()->GetGamePad().IsRightTriggerPressed())
	{
		VFly(mCameraMovementRate * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
	}

	if (InputManager::GetInstance()->GetKeyboard()->IsKeyDown(DIK_E) || InputManager::GetInstance()->GetGamePad().IsLeftTriggerPressed())
	{
		VFly(-mCameraMovementRate * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
	}
	if (InputManager::GetInstance()->GetKeyboard()->IsKeyDown(DIK_R) || InputManager::GetInstance()->GetGamePad().IsStartPressed())
	{
		ResetCameraTransform();
	}

	if (InputManager::GetInstance()->GetMouse()->IsButtonDown(MouseButtonLeft))
	{
		float currYRotAngle = XMConvertToRadians(static_cast<float>(InputManager::GetInstance()->GetMouse()->GetCurrentState()->lX) * static_cast<float>(mCameraSensitivity) * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
		float currXRotAngle = XMConvertToRadians(static_cast<float>(InputManager::GetInstance()->GetMouse()->GetCurrentState()->lY) * static_cast<float>(mCameraSensitivity) * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
		Pitch(currXRotAngle);
		Yaw(currYRotAngle);
	}

	if (InputManager::GetInstance()->GetGamePad().IsRightThumbStickUp() || InputManager::GetInstance()->GetGamePad().IsRightThumbStickDown())
	{
		float currXRotAngle = XMConvertToRadians(static_cast<float>(InputManager::GetInstance()->GamePadRightStickYValue()) * static_cast<float>(mCameraSensitivity) * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
		Pitch(-currXRotAngle);
	}

	if (InputManager::GetInstance()->GetGamePad().IsRightThumbStickLeft() || InputManager::GetInstance()->GetGamePad().IsRightThumbStickRight())
	{
		float currYRotAngle = XMConvertToRadians(static_cast<float>(InputManager::GetInstance()->GamePadRightStickXValue()) * static_cast<float>(mCameraSensitivity) * static_cast<float>(GameTime::GetInstance()->DeltaTime()));
		Yaw(currYRotAngle);
	}

	UpdateFPSViewMatrix();
}

void ThirdPersonCamera::ResetCameraTransform()
{
	SetRight(1.0f, 0.0f, 0.0f);
	SetUp(0.0f, 1.0f, 0.0f);
	SetLook(0.0f, 0.0f, 1.0f);
	SetPosition(0.0f, 30.0f, -20.0f);
}

void ThirdPersonCamera::LookAt(const XMFLOAT3& target)
{
	mTarget = target;
}

//Called once
void ThirdPersonCamera::LookAt(const XMFLOAT3& currCamPos, const XMFLOAT3& currTargetPos, const XMFLOAT3& currUpDir)
{
	SetPosition(currCamPos);
	SetTarget(currTargetPos);
	SetTargetYAxis(currUpDir);

	SetLookXM(XMVectorSubtract(GetTargetXM(), GetPositionXM()));
	SetLookXM(XMVector3Normalize(GetLookXM()));

	XMVECTOR currUpVec = XMVectorSet(currUpDir.x, currUpDir.y, currUpDir.z, 0.0f);
	SetRightXM(XMVector3Cross(currUpVec, GetLookXM()));
	SetRightXM(XMVector3Normalize(GetRightXM()));

	SetUpXM(XMVector3Cross(GetLookXM(), GetRightXM()));
	SetUpXM(XMVector3Normalize(GetUpXM()));
	SetRightXM(XMVector3Normalize(GetRightXM()));

	XMStoreFloat4x4(&mView, XMMatrixIdentity());

	XMVECTOR currCamPosVec = XMVectorSet(currCamPos.x, currCamPos.y, currCamPos.z, 1.0f);
	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = -XMVectorGetX(XMVector3Dot(GetRightXM(), currCamPosVec));

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = -XMVectorGetX(XMVector3Dot(GetUpXM(), currCamPosVec));

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = -XMVectorGetX(XMVector3Dot(GetLookXM(), currCamPosVec));

	XMMATRIX finalOrientationMX;
	finalOrientationMX = XMLoadFloat4x4(&mView);
	SetOrientationXM(XMQuaternionRotationMatrix(finalOrientationMX));

	XMVECTOR offSetVec = XMVectorSubtract(GetTargetXM(), GetPositionXM());
	SetOffsetDistance(XMVectorGetX(XMVector3Length(offSetVec)));
}

void ThirdPersonCamera::UpdateOrientation()
{
	mPitchDegrees *= static_cast<float>(GameTime::GetInstance()->DeltaTime());
	mHeadingDegrees *= static_cast<float>(GameTime::GetInstance()->DeltaTime());

	float heading = XMConvertToRadians(mHeadingDegrees);
	float pitch = XMConvertToRadians(mPitchDegrees);

	XMVECTOR rotationQuat;

	if (heading != 0.0f)
	{
		rotationQuat = XMQuaternionRotationAxis(GetTargetYAxisXM(), heading);
		SetOrientationXM(XMQuaternionMultiply(rotationQuat, GetOrientationXM()));
	}

	if (pitch != 0.0f)
	{
		XMVECTOR worldYAxis{ 0.0f, 1.0f, 0.0f };
		rotationQuat = XMQuaternionRotationAxis(worldYAxis, pitch);
		SetOrientationXM(XMQuaternionMultiply(GetOrientationXM(), rotationQuat));
	}
}

void ThirdPersonCamera::UpdateViewMatrix()
{
	SetOrientationXM(XMQuaternionNormalize(GetOrientationXM()));
	XMMATRIX orientationMatrixXM = XMMatrixRotationQuaternion(GetOrientationXM());
	XMStoreFloat4x4(&mView, orientationMatrixXM);

	SetRight(XMFLOAT3(mView(0, 0), mView(1, 0), mView(2, 0)));
	SetUp(XMFLOAT3(mView(0, 1), mView(1, 1), mView(2, 1)));
	SetLook(XMFLOAT3(mView(0, 2), mView(1, 2), mView(2, 2)));

	XMVECTOR zAxisOffsetVec = XMVectorScale(GetLookXM(), -mOffsetDistance * 3.5f);
	XMVECTOR cameraPosVec = XMVectorAdd(zAxisOffsetVec, GetTargetXM());
	cameraPosVec.m128_f32[1] += 80.0f;
	SetPositionXM(cameraPosVec);

	//(Camera's Position)
	float newXPos = -XMVectorGetX(XMVector3Dot(GetPositionXM(), GetRightXM()));
	float newYPos = -XMVectorGetX(XMVector3Dot(GetPositionXM(), GetUpXM()));
	float newZPos = -XMVectorGetX(XMVector3Dot(GetPositionXM(), GetLookXM()));

	mView(3, 0) = newXPos;
	mView(3, 1) = newYPos;
	mView(3, 2) = newZPos;
}