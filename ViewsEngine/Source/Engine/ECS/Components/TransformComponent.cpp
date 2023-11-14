#include "Chokbar.h"

#include "TransformComponent.h"

Transform::Transform()
{
	// Initialize orientation vectors (right, up, forward)
	m_Right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_Forward = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_pParent = nullptr;
	m_pChildren = std::vector<Transform *>();

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
	DirectX::XMStoreFloat4x4(&m_TransposedParentedWorldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_ParentedWorldMatrix, DirectX::XMMatrixIdentity());

	// Initialize parented world matrix as an identity matrix
	DirectX::XMStoreFloat4x4(&m_ParentedWorldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&m_TransposedParentedWorldMatrix, DirectX::XMMatrixIdentity());

	m_Dirty = false;
}

Transform::~Transform()
{
	NULL(m_pParent);


	for (auto child : m_pChildren)
	{
		NULL(child);
	}
}

void Transform::OnRemovedComponent()
{
	NULL(m_pParent);

	for (auto child : m_pChildren)
	{
		NULL(child);
	}
}

void Transform::Translate(float x, float y, float z, Space space)
{
	switch (space)
	{
	case Transform::Local:
		// Create a translation vector in local space
		DirectX::XMFLOAT3 translation;
		translation.x = x;
		translation.y = y;
		translation.z = z;

		// Transform the translation vector into world space
		DirectX::XMVECTOR translationVector = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&translation), DirectX::XMLoadFloat4x4(&m_RotationMatrix));

		// Update the position in world space
		m_Position.x += DirectX::XMVectorGetX(translationVector);
		m_Position.y += DirectX::XMVectorGetY(translationVector);
		m_Position.z += DirectX::XMVectorGetZ(translationVector);
		break;

	case Transform::World:
		// Update the position vector
		m_Position.x += x;
		m_Position.y += y;
		m_Position.z += z;
		break;
	}

	// Update the position matrix
	UpdatePositionMatrix();
}

void Transform::Translate(DirectX::XMFLOAT3 translation, Space space)
{
	Translate(translation.x, translation.y, translation.z, space);
}

void Transform::Translate(DirectX::XMVECTOR translation, Space space)
{
	DirectX::XMFLOAT3 translationFloat3;
	DirectX::XMStoreFloat3(&translationFloat3, translation);

	Translate(translationFloat3, space);
}

void Transform::SetPosition(float x, float y, float z)
{
	// Set the position directly
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
	UpdatePositionMatrix();
}
void Transform::SetPosition(DirectX::XMFLOAT3 newPosition)
{
	SetPosition(newPosition.x, newPosition.y, newPosition.z);
}

void Transform::RotateFromAxisAngle(DirectX::XMFLOAT3 axis, float angle)
{
	DirectX::XMVECTOR rotationDelta = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axis), DirectX::XMConvertToRadians(angle));
	DirectX::XMVECTOR currentRotation = DirectX::XMLoadFloat4(&m_RotationQuaternion);
	currentRotation = DirectX::XMQuaternionMultiply(currentRotation, rotationDelta);

	// Store the result back in rotationQuaternion as XMFLOAT4
	DirectX::XMStoreFloat4(&m_RotationQuaternion, currentRotation);
	// Store the result back in rotationQuaternion as XMFLOAT4
	UpdateRotationMatrix();

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
void Transform::RotateYaw(float angle, Space space)
{
	DirectX::XMFLOAT3 axis = space == Space::Local ? m_Up : DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	RotateFromAxisAngle(axis, angle);
}
void Transform::RotatePitch(float angle, Space space)
{
	DirectX::XMFLOAT3 axis = space == Space::Local ? m_Right : DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	RotateFromAxisAngle(axis, angle);
}
void Transform::RotateRoll(float angle, Space space)
{
	DirectX::XMFLOAT3 axis = space == Space::Local ? m_Forward : DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
	RotateFromAxisAngle(axis, angle);
}
void Transform::Rotate(float pitch = 0.f, float yaw = 0.f, float roll = 0.f, Space space)
{
	RotatePitch(pitch, space);
	RotateYaw(yaw, space);
	RotateRoll(roll, space);
}
void Transform::Rotate(DirectX::XMFLOAT3 rotation, Space space)
{
	Rotate(rotation.x, rotation.y, rotation.z, space);
}

void Transform::SetScale(float scale)
{
	SetScale(scale, scale, scale);
}

void Transform::SetScale(float x, float y, float z)
{
	// Set the scale factors directly
	m_Scale.x = x;
	m_Scale.y = y;
	m_Scale.z = z;

	UpdateScaleMatrix();
}
void Transform::SetScale(DirectX::XMFLOAT3 scaleFactors)
{
	SetScale(scaleFactors.x, scaleFactors.y, scaleFactors.z);
}

void Transform::LookAt(const DirectX::XMFLOAT3 target)
{
	DirectX::XMVECTOR directionVector = XMVectorSubtract(XMLoadFloat3(&target), XMLoadFloat3(&m_Position));
	directionVector = XMVector3Normalize(directionVector);

	float pitch = 0.0f;
	float yaw = 0.0f;
	float roll = 0.0f;

	if (XMVector3NotEqual(directionVector, XMVectorZero()))
	{
		pitch = asin(XMVectorGetY(directionVector));
		yaw = atan2(XMVectorGetX(directionVector), XMVectorGetZ(directionVector));
	}

	pitch = XMConvertToDegrees(pitch);
	yaw = XMConvertToDegrees(yaw);
	roll = XMConvertToDegrees(roll);

	Rotate(DirectX::XMFLOAT3(pitch, yaw, roll));
}

void Transform::SetParent(Transform *pParent)
{
	m_pParent = pParent;
	pParent->m_pChildren.push_back(this);
}

DirectX::XMFLOAT3 Transform::GetEulerAngles()
{
	// Extract the Euler angles from the rotation matrix
	float pitch = asin(-m_Forward.y);
	float yaw = atan2(m_Forward.x, m_Forward.z);
	float roll = atan2(m_Up.x, m_Right.x);

	// Convert the angles to degrees if desired
	float pitch_deg = DirectX::XMConvertToDegrees(pitch);
	float yaw_deg = DirectX::XMConvertToDegrees(yaw);
	float roll_deg = DirectX::XMConvertToDegrees(roll);

	// Return the Euler angles
	return DirectX::XMFLOAT3(pitch_deg, yaw_deg, roll_deg);
}

DirectX::XMFLOAT3 Transform::GetWorldPosition() const
{
	XMFLOAT3 worldPosition;
	worldPosition.x = m_ParentedWorldMatrix._41;
	worldPosition.y = m_ParentedWorldMatrix._42;
	worldPosition.z = m_ParentedWorldMatrix._43;

	return worldPosition;
}

void Transform::UpdatePositionMatrix()
{
	m_Dirty = true;

	// Create the position matrix
	DirectX::XMStoreFloat4x4(&m_PositionMatrix, DirectX::XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z));
}
void Transform::UpdateRotationMatrix()
{
	m_Dirty = true;
	// Create a quaternion for this rotation
	DirectX::XMVECTOR quaternion = DirectX::XMLoadFloat4(&m_RotationQuaternion);
	// Create a quaternion for this rotation
	// Convert the quaternion to a rotation matrix
	DirectX::XMStoreFloat4x4(&m_RotationMatrix, DirectX::XMMatrixRotationQuaternion(quaternion));
}
void Transform::UpdateScaleMatrix()
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

void Transform::UpdateWorldMatrix()
{
	// Create the position matrix
	// Combine rotation and scale and position
	DirectX::XMMATRIX newWorldMatrix = DirectX::XMLoadFloat4x4(&m_RotationMatrix) * DirectX::XMLoadFloat4x4(&m_ScaleMatrix) * DirectX::XMLoadFloat4x4(&m_PositionMatrix);
	// Combine rotation and scale with position
	// Convert the final world matrix to XMFLOAT4X4
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, newWorldMatrix);
}

void Transform::UpdateParentedWorldMatrix()
{
	// if (m_Dirty || m_pParent && m_pParent->IsDirty())
	{
		DirectX::XMFLOAT4X4 parentWorldMatrix;

		if (m_pParent)
		{
			m_pParent->UpdateParentedWorldMatrix();
			parentWorldMatrix = *m_pParent->GetParentedWorldMatrix();
		}
		else
		{
			XMStoreFloat4x4(&parentWorldMatrix, DirectX::XMMatrixIdentity());
		}

		UpdateWorldMatrix();
		XMStoreFloat4x4(&m_ParentedWorldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&m_WorldMatrix), XMLoadFloat4x4(&parentWorldMatrix)));
		XMStoreFloat4x4(&m_TransposedParentedWorldMatrix, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ParentedWorldMatrix)));

		m_Dirty = false;
	}
}

Transform *Transform::GetChild(const char *str) const
{
	for (const auto child : m_pChildren)
	{
		if (child->gameObject->GetName() == str)
		{
			return child;
		}
	}

	return nullptr;
}
