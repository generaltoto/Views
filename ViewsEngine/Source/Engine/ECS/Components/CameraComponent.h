#pragma once
#include <DirectXMath.h>

#include "Engine/Core/D3D/Base/D3DUtils.h"
#include "Engine/ECS/Components/Component.h"


class CameraComponent : public Component
{
public:
	CameraComponent();
	~CameraComponent() override;

	void OnAddedComponent() override;

	// Get camera basis vectors.
	DirectX::XMVECTOR GetRight() const;
	DirectX::XMFLOAT3 GetRight3f() const;
	DirectX::XMVECTOR GetUp() const;
	DirectX::XMFLOAT3 GetUp3f() const;
	DirectX::XMVECTOR GetLook() const;
	DirectX::XMFLOAT3 GetLook3f() const;

	// Get frustum properties.
	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	void SetWindowWidth(float width) { m_windowWidth = width; UpdateWindowWithNewRange(); }
	void SetWindowHeight(float height) { m_windowHeight = height; UpdateWindowWithNewRange(); }

	// Set frustum.
	void SetFOV(float fovY);
	void SetAspect(float aspect);
	void SetZRange(float zn, float zf);
	void SetLens(float fovY, float aspect, float zn, float zf);

	// Define camera space via LookAt parameters.
	void LookAt(DirectX::XMFLOAT3 targetPos);


	// Get View/Proj matrices.
	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;
	DirectX::XMMATRIX GetViewProj()const;

	DirectX::XMMATRIX GetOrthoProj()const;
	DirectX::XMMATRIX GetOrthoView()const;
	DirectX::XMMATRIX GetOrthoViewProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;
	DirectX::XMFLOAT4X4 GetViewProj4x4f()const; 

	DirectX::XMFLOAT4X4 GetOrthoProj4x4f()const;
	DirectX::XMFLOAT4X4 GetOrthoView4x4f()const;
	DirectX::XMFLOAT4X4 GetOrthoViewProj4x4f()const;


	// Strafe/Walk the camera a distance d.
	void Strafe(float d);
	void Walk(float d);

	// Rotate the camera.
	void Pitch(float angle);
	void RotateY(float angle);



	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();

private:

	void __vectorcall LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);

	void UpdateWindowWithNewRange();
	void UpdateProjectionMatrix();

private:

	const DirectX::XMVECTOR m_WorldUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);


	// Cache frustum properties.
	float m_NearZ = 0.5f;
	float m_FarZ = 1000.0f;
	float m_Aspect = 0.0f;
	float m_FovY = 75.0f;
	float m_windowWidth = 0.0f;
	float m_NearWindowHeight = 0.0f;
	float m_windowHeight = 0.0f;
	float m_FarWindowHeight = 0.0f;

	bool m_ViewDirty;

	// Cache View/Proj matrices.
	DirectX::XMFLOAT4X4 m_View = Identity4X4();
	DirectX::XMFLOAT4X4 m_OthoView = Identity4X4();
	DirectX::XMFLOAT4X4 m_Proj = Identity4X4();
	DirectX::XMFLOAT4X4 m_OrthoProj = Identity4X4();
};