#pragma once

struct InstanceData
{
	DirectX::XMFLOAT4X4 World = Identity4X4();
	DirectX::XMFLOAT4 Color1 = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	DirectX::XMFLOAT4 Color2 = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	float AgeRatio = 0.0f;
};

// ParticleTransform class that holds the position, rotation and scale of a particle
// Note that we do not used the Transform class, we do not need to component part of the transform
class VGParticleTransform
{
public:
	VGParticleTransform();
	~VGParticleTransform();

public:
	void Translate(float x, float y, float z);
	void Translate(DirectX::XMFLOAT3 translation);

	void RotateYaw(float angle);
	void RotatePitch(float angle);
	void RotateRoll(float angle);

	void Rotate(float yaw, float pitch, float roll);
	void Rotate(DirectX::XMFLOAT3 rotation);

	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);

	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);

	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);

	DirectX::XMFLOAT3 GetPosition() const {return m_Position;}
	DirectX::XMFLOAT3 GetScale() const { return m_Scale; }
	DirectX::XMFLOAT4 GetQuaternion() const { return m_RotationQuaternion; }

	DirectX::XMFLOAT4X4* GetWorldMatrix() { return &m_WorldMatrix ;}

	[[nodiscard]] bool IsDirty() const { return m_Dirty; }

	void UpdateWorldMatrix();

private:

	void RotateFromAxisAngle(DirectX::XMFLOAT3 axis, float angle);

	void UpdatePositionMatrix();
	void UpdateRotationMatrix();
	void UpdateScaleMatrix();

	bool m_Dirty;

	DirectX::XMFLOAT3 m_Right;
	DirectX::XMFLOAT3 m_Up;
	DirectX::XMFLOAT3 m_Forward;

	DirectX::XMFLOAT3 m_Position;
	DirectX::XMFLOAT4X4 m_PositionMatrix;

	DirectX::XMFLOAT3 m_Scale;
	DirectX::XMFLOAT4X4 m_ScaleMatrix;

	DirectX::XMFLOAT4X4 m_RotationMatrix;
	DirectX::XMFLOAT4 m_RotationQuaternion;

	DirectX::XMFLOAT4X4 m_WorldMatrix;
};

// Particle class that holds all the data for a single particle
// This class is only used by the ParticleRenderer class 
class VGParticle
{
public:
	VGParticle();
	~VGParticle();

	void Update(float deltaTime);

	[[nodiscard]] bool IsAlive() const { return CurrentLifeTime < LifeTime; }
	[[nodiscard]] bool IsActive() const { return m_isActive; }

	void Sleep() { m_isActive = false; }
	void Awake() { m_isActive = true; }

	void Reset();
	void Init(float rLifeTime, DirectX::XMFLOAT3 rVel, DirectX::XMFLOAT3 rAngVel, DirectX::XMFLOAT3 parentPos);
	
	float CurrentLifeTime;
	float LifeTime;

	DirectX::XMFLOAT3 Velocity;
	DirectX::XMFLOAT3 AngularVelocity;

	VGParticleTransform* Transform;

private:
	bool m_isActive;
};
