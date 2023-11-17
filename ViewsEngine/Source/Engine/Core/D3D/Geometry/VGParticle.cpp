

#include "..\Base\VGUtils.h"

#include "VGParticle.h"

#include <cmath>
using namespace DirectX;

#pragma region ParticleTransform
VGParticleTransform::VGParticleTransform() : m_Dirty(false)
{
	// Initialize orientation vectors (right, up, forward)
	m_Right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_Forward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	// Initialize position, scale, and rotation
	m_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	DirectX::XMStoreFloat4x4(&m_PositionMatrix, DirectX::XMMatrixIdentity());
	m_Scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	DirectX::XMStoreFloat4x4(&m_ScaleMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_RotationMatrix, DirectX::XMMatrixIdentity());
	m_RotationQuaternion = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	// Initialize rotation matrix as an identity matrix
	// Initialize world matrix as an identity matrix
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity());
}

VGParticleTransform::~VGParticleTransform()
{
}

void VGParticleTransform::Translate(float x, float y, float z)
{
	// Update the position vector
	m_Position.x += x;
	m_Position.y += y;
	m_Position.z += z;
	UpdatePositionMatrix();
}

void VGParticleTransform::Translate(DirectX::XMFLOAT3 translation)
{
	Translate(translation.x, translation.y, translation.z);
}

void VGParticleTransform::SetPosition(float x, float y, float z)
{
	// Set the position directly
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
	UpdatePositionMatrix();
}

void VGParticleTransform::SetPosition(DirectX::XMFLOAT3 newPosition)
{
	SetPosition(newPosition.x, newPosition.y, newPosition.z);
}

void VGParticleTransform::RotateFromAxisAngle(DirectX::XMFLOAT3 axis, float angle)
{
	DirectX::XMVECTOR rotationDelta = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axis), DirectX::XMConvertToRadians(angle));
	DirectX::XMVECTOR currentRotation = DirectX::XMLoadFloat4(&m_RotationQuaternion);
	currentRotation = DirectX::XMQuaternionMultiply(currentRotation, rotationDelta);

	// Store the result back in rotationQuaternion as XMFLOAT4
	DirectX::XMStoreFloat4(&m_RotationQuaternion, currentRotation);
	// Store the result back in rotationQuaternion as XMFLOAT4
	UpdateRotationMatrix();
	// Recalculate the world matrix
	m_Right.x = m_RotationMatrix._11;
	m_Right.y = m_RotationMatrix._12;
	m_Right.z = m_RotationMatrix._13;
	m_Up.x = m_RotationMatrix._21;
	m_Up.y = m_RotationMatrix._22;
	m_Up.z = m_RotationMatrix._23;
	m_Forward.x = m_RotationMatrix._31;
	m_Forward.y = m_RotationMatrix._32;
	m_Forward.z = m_RotationMatrix._33;
}

void VGParticleTransform::RotateYaw(float angle)
{
	RotateFromAxisAngle(m_Up, angle);
}

void VGParticleTransform::RotatePitch(float angle)
{
	RotateFromAxisAngle(m_Right, angle);
}

void VGParticleTransform::RotateRoll(float angle)
{
	RotateFromAxisAngle(m_Forward, angle);
}

void VGParticleTransform::Rotate(float pitch = 0.f, float yaw = 0.f, float roll = 0.f)
{
	RotatePitch(pitch);
	RotateYaw(yaw);
	RotateRoll(roll);
}

void VGParticleTransform::Rotate(DirectX::XMFLOAT3 rotation)
{
	Rotate(rotation.x, rotation.y, rotation.z);
}

void VGParticleTransform::Scale(float x, float y, float z)
{
	// Update the scale factors
	m_Scale.x *= x;
	m_Scale.y *= y;
	m_Scale.z *= z;
	UpdateScaleMatrix();
}

void VGParticleTransform::Scale(DirectX::XMFLOAT3 scaleFactors)
{
	Scale(scaleFactors.x, scaleFactors.y, scaleFactors.z);
}

void VGParticleTransform::SetScale(float x, float y, float z)
{
	// Set the scale factors directly
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;

	UpdateScaleMatrix();
}

void VGParticleTransform::SetScale(DirectX::XMFLOAT3 scaleFactors)
{
	SetScale(scaleFactors.x, scaleFactors.y, scaleFactors.z);
}

void VGParticleTransform::UpdatePositionMatrix()
{
	m_Dirty = true;

	// Create the position matrix
	DirectX::XMStoreFloat4x4(&m_PositionMatrix, DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z));
}

void VGParticleTransform::UpdateRotationMatrix()
{
	m_Dirty = true;
	// Create a quaternion for this rotation
	DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&m_RotationQuaternion);
	// Create a quaternion for this rotation
	// Convert the quaternion to a rotation matrix
	DirectX::XMStoreFloat4x4(&m_RotationMatrix, DirectX::XMMatrixRotationQuaternion(quaternion));
}

void VGParticleTransform::UpdateScaleMatrix()
{
	m_Dirty = true;
	// Convert the quaternion to a rotation matrix
	// Scale the orientation vectors
	DirectX::XMVECTOR scaledRight = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&m_Right), m_Scale.x);
	DirectX::XMVECTOR scaledUp = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&m_Up), m_Scale.y);
	DirectX::XMVECTOR scaledForward = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&m_Forward), m_Scale.z);

	// Create the scale matrix
	m_ScaleMatrix = DirectX::XMFLOAT4X4(
		m_Scale.x, 0.0f, 0.0f, 0.0f,
		0.0f, m_Scale.y, 0.0f, 0.0f,
		0.0f, 0.0f, m_Scale.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

void VGParticleTransform::UpdateWorldMatrix()
{
	// Create the position matrix
	// Combine rotation and scale and position
	DirectX::XMMATRIX newWorldMatrix = DirectX::XMLoadFloat4x4(&m_RotationMatrix) * DirectX::XMLoadFloat4x4(&m_ScaleMatrix) * DirectX::XMLoadFloat4x4(&m_PositionMatrix);
	// Combine rotation and scale with position
	// Convert the final world matrix to XMFLOAT4X4
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixTranspose(newWorldMatrix));

	m_Dirty = false;
}
#pragma endregion

#pragma region Particle
VGParticle::VGParticle()
	: LifeTime(0.0f), CurrentLifeTime(0.0f), Velocity(XMFLOAT3(0.0f, 0.0f, 0.0f)), AngularVelocity(XMFLOAT3(0.0f, 0.0f, 0.0f)), m_isActive(false)
{
	Transform = new VGParticleTransform();
}

VGParticle::~VGParticle()
{
	DELPTR(Transform);
}

void VGParticle::Update(float deltaTime)
{
	CurrentLifeTime -= deltaTime;
}

void VGParticle::Reset()
{
	CurrentLifeTime = 0.0f;

	AngularVelocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	Velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	Transform->SetPosition(0.0f, 0.0f, 0.0f);
	Transform->SetScale(1.0f, 1.0f, 1.0f);
	Transform->Rotate(0.0f, 0.0f, 0.0f);
}

void VGParticle::Init(float rLifeTime, DirectX::XMFLOAT3 rVel, DirectX::XMFLOAT3 rAngVel, DirectX::XMFLOAT3 parentPos)
{
	LifeTime = rLifeTime;
	CurrentLifeTime = 0.0f;

	AngularVelocity = rAngVel;
	Velocity = rVel;

	Transform->SetPosition(parentPos);
}
#pragma endregion