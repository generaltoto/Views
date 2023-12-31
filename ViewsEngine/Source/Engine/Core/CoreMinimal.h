#pragma once

#include "CoreDefinitions.h"
#include "DebugUtils.h"
#include "PerGameSettings.h"

#include "Common/Logger.h"
#include "Common/Time.h"

#include "D3D/Base/VGMeshType.h"
#include "D3D/Base/VGMaterialType.h"


// Resources includes
#include "Engine/Resource.h"

// Components includes
#include "Engine/ECS/Base/GameObject.h"
#include "Engine/ECS/Components/Component.h"
#include "Engine/ECS/Components/TransformComponent.h"
#include "Engine/ECS/Components/CameraComponent.h"
#include "Engine/ECS/Components/CustomComponent.h"
#include "Engine/ECS/Components/Collision/Rigidbody.h"
#include "Engine/ECS/Components/Collision/SphereCollider.h"

// Managers includes
#include "Engine/Managers/TimeManager.h"
#include "Engine/Managers/CameraManager.h"

// D3D includes
#include "D3D/Renderers/VGMeshRenderer.h"
#include "D3D/Renderers/VGParticleRenderer.h"
#include "D3D/Renderers/VGSkyRenderer.h"
#include "D3D/Renderers/VGUiRenderer.h"
#include "D3D/Shaders/Textures/VGTexture2D.h"
#include "D3D/Shaders/Base/VGMaterial.h"
#include "D3D/Shaders/Textures/VGCubemap.h"

#include "Engine/Managers/InputHandler.h"

#ifdef WIN32
#include "Platform/Windows/Win32Utils.h"
#include "Platform/Windows/Window.h"
#include "Platform/Windows/IApplication.h"
#endif