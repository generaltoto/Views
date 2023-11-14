#pragma once

class CameraComponent;

class CameraManager
{
public :

	CameraManager();
	~CameraManager();

	static void SetMainCamera(CameraComponent* camera);
	static CameraComponent* GetMainCamera() { return m_MainCamera; }

private:


	static CameraComponent* m_MainCamera;

};