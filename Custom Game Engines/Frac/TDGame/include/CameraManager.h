#pragma once
#include "Core\EngineCore.h"
#include "CoreRenderAPI\Components\Camera.h"
#include "CoreRenderAPI\Components\Transform.h"

//////// Will be replaced when an interface is provided to get these values from
#define screenHeight 720
#define screenWidth 1280

class CameraManager
{
public:
	CameraManager(Frac::EngineCore& engine);
	~CameraManager();

	enum struct ECameraBehaviour {
		DefenseGrid,
		Static,
		Noclip,
		Demo
	};
	struct CameraTrait{
		ECameraBehaviour	CamBehaviour;
		glm::vec3			LookAt;
		float				NoclipSpeed;
		float				NoclipSpeedMultiplier;
		glm::vec3			Offset;
		glm::vec2			MinMaxZoom = {5.0f,20.f};
		float				MoveSpeed = 10.f;
		float				ZoomSpeed = 2.f;
		
		float				DemoTime = 0.f;
		glm::vec3			DemoSpeed = glm::vec3(10.f, 8.f, 10.f);
	};

	void Initialize();
	
	Frac::EntityHandle CreateCamera(
		ECameraBehaviour camBehaviour = ECameraBehaviour::Static, 
		bool makeActiveCam = true, 
		TOR::Transform transform = TOR::Transform({glm::vec3(1.f), glm::vec3(1.f), glm::quat(0.f, 0.f, 0.f, 0.f), glm::vec3(0.f)}), 
		glm::vec3 lookAt = glm::vec3(0.f), 
		TOR::Camera camValues = TOR::Camera({glm::perspectiveFovRH(glm::radians<float>(70.0f),static_cast<float>(screenWidth), static_cast<float>(screenHeight), 0.01f, 1000.f), 70.0f, 0.01f, 1000.0f,  static_cast<float>(screenWidth / screenHeight), static_cast<int>(screenWidth), static_cast<int>(screenHeight), TOR::ECameraType::Perspective, false, true}));

	void SetMainCameraEntity(EntityHandle newMain);

	Frac::EntityHandle GetMainCameraEntity() const;
	const glm::mat4 GetMainCameraEntityVP() const;

	std::vector<Frac::EntityHandle> GetCameraEntities(bool dontGetMain = false);

	Frac::System<TOR::Transform, TOR::Camera, CameraTrait> m_updateMainCameraS;
	
private:
	void UpdateMainCameraEntity(Frac::EntityHandle, TOR::Transform&, TOR::Camera&, CameraTrait&);

	void UpdateDefenseGrid(Frac::EntityHandle, TOR::Transform&, TOR::Camera&, CameraTrait&);
	void UpdateNoclip(Frac::EntityHandle, TOR::Transform&, TOR::Camera&, CameraTrait&);
	void UpdateDemo(Frac::EntityHandle, TOR::Transform&, TOR::Camera&, CameraTrait&); /*! <- used for main menu fly-bys*/

	Frac::EngineCore& m_engine;
	Frac::ECSWrapper& m_world;
	Frac::InputSystem& m_inputSystem;

	Frac::EntityHandle m_mainCamera;

	glm::vec3 m_upVec;

	Frac::Query<TOR::Camera, CameraTrait> m_camEntsQuery;

	int m_camCounter;
	// values used to set the new active cams
	bool m_activeCamHasChanged = false;
	EntityHandle m_newActiveCamID = 0;
	
};