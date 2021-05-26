#include "pch.h"
#include "CameraManager.h"
CameraManager::CameraManager(Frac::EngineCore& engine)
	: m_engine(engine)
	, m_world(engine.GetWorld())
	, m_inputSystem(engine.GetInputSystem())
	, m_updateMainCameraS{ engine.GetWorld().makeSystem<TOR::Transform, TOR::Camera, CameraTrait>() }
	, m_mainCamera(0)
	, m_upVec(glm::vec3(0.f, 1.f, 0.f))
	, m_camEntsQuery(m_world.makeQuery<TOR::Camera, CameraTrait>())
	, m_camCounter(0)
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::Initialize()
{
	std::function<void(Frac::EntityHandle, TOR::Transform&, TOR::Camera&, CameraTrait&)> func
	{
		[this](Frac::EntityHandle e, TOR::Transform& transform, TOR::Camera& camera, CameraTrait& camTrait)
		{
			UpdateMainCameraEntity(e, transform, camera, camTrait);
		}
	};

	m_updateMainCameraS = m_engine.GetWorld().makeSystem<TOR::Transform, TOR::Camera, CameraTrait>(func, Frac::ePipelineStage::onUpdate, "UpdateMainCameraEntity");

	m_inputSystem.AddVirtualJoystick(); // call somewhere else?
	
	LOGINFO("Camera Manager Initialized");
}

Frac::EntityHandle CameraManager::CreateCamera(ECameraBehaviour camBehaviour, bool makeActiveCam, TOR::Transform transform, glm::vec3 lookAt, TOR::Camera camValues)
{
	Frac::EntityHandle camEnt = m_world.makeEntity<TOR::Transform, TOR::Camera, CameraTrait>(
		"CamEnt_" + std::to_string(m_camCounter++)
		, {
			transform.Position
			, transform.Scale
			, transform.Orientation
			, transform.EulerOrientation
			, transform.ModelTransformData
			, transform.isDirty
		}
		, { 
			camValues.ProjectionMat
			, camValues.FovInDeg
			, camValues.ZNear
			, camValues.ZFar
			, camValues.AspectRatio
			, camValues.ScreenWidth
			, camValues.ScreenHeight
			, camValues.CameraType
			, makeActiveCam
			, camValues.IsDirty
		}
		, { camBehaviour
			, lookAt
			, 4.5f
			, 2.5f
			, (lookAt - transform.Position)}
	);
	
	if (makeActiveCam)
	{
		m_mainCamera = camEnt;
	}

	return camEnt;
}

Frac::EntityHandle CameraManager::GetMainCameraEntity() const
{
	return m_mainCamera;
}

const glm::mat4 CameraManager::GetMainCameraEntityVP() const
{
	if(!m_world.has<TOR::Transform>(m_mainCamera))
	{
		LOGWARNING("CameraManager | mainCamera has no Transform!");
		return glm::mat4(0.f);
	}
	
	const TOR::Transform* camTrans	= m_world.get<TOR::Transform>(m_mainCamera);
	const CameraTrait* camTrait		= m_world.get<CameraTrait>(m_mainCamera);

	const glm::mat4 viewMatrix = glm::lookAtRH(
		camTrans->Position, 
		camTrait->LookAt, 
		m_upVec);

	return (m_world.get<TOR::Camera>(m_mainCamera)->ProjectionMat * viewMatrix);
}

std::vector<Frac::EntityHandle> CameraManager::GetCameraEntities(bool dontGetMain)
{
	std::vector<Frac::EntityHandle> camEnts;
	std::function<void(Frac::It, uint64_t, TOR::Camera*, CameraTrait*)> func = [&camEnts, dontGetMain](Frac::It it, uint64_t index, TOR::Camera* camera, CameraTrait* camTrait)
	{
		if ((dontGetMain && !camera[index].IsActive) || !dontGetMain)
		{
			camEnts.emplace_back(it.m_it.entity(index).id());
		}
	};
	m_camEntsQuery.Iter(func);

	return camEnts;
}

void CameraManager::SetMainCameraEntity(EntityHandle newMain)
{
	if (!m_world.has<CameraTrait>(newMain))
	{
		LOGWARNING("CameraManager | Cannot set entity without a camera component as the main camera!");
		return;
	}
	const auto oldCam = m_world.get<TOR::Camera>(m_mainCamera);
	oldCam->IsActive = false;
	
	m_activeCamHasChanged = true;
	m_newActiveCamID = newMain;
	m_mainCamera = newMain;
}

void CameraManager::UpdateMainCameraEntity(Frac::EntityHandle e, TOR::Transform& transform, TOR::Camera& camera, CameraTrait& camTrait)
{
	if(m_activeCamHasChanged && e == m_newActiveCamID)
	{
		m_activeCamHasChanged = false;
		m_newActiveCamID = 0;
		camera.IsActive = true;
	}
	if(m_mainCamera != e)
	{
		return;
	}
//	printf("Name of entity: %s \n", m_world.getName(e).c_str());

	switch (camTrait.CamBehaviour)
	{
	case ECameraBehaviour::DefenseGrid:

		UpdateDefenseGrid(e, transform, camera, camTrait);

		break;
	case ECameraBehaviour::Noclip:

		UpdateNoclip(e, transform, camera, camTrait);

		break;
	case ECameraBehaviour::Static:
		break;
	case ECameraBehaviour::Demo:

		UpdateDemo(e, transform, camera, camTrait);

		break;
	}

	camera.ProjectionMat = glm::perspectiveFovRH(
		glm::radians<float>(camera.FovInDeg), 
		static_cast<float>(camera.ScreenWidth), 
		static_cast<float>(camera.ScreenHeight), camera.ZNear, camera.ZFar);

#if defined(CONFIG_DEBUG) || defined(CONFIG_DEVELOP)
	// quick and dirty debug code
	auto camEnts = GetCameraEntities();
	if ((m_inputSystem.GetKeyOnce(Frac::KEY_1) || m_inputSystem.GetJoystickComboOnce(m_inputSystem.GetDefaultJoystick(), Frac::JOYSTICK_BUTTON_LB, Frac::JOYSTICK_BUTTON_DPAD_LEFT)) && !camEnts.empty())
	{
		SetMainCameraEntity(camEnts[0]);
	}
	else if ((m_inputSystem.GetKeyOnce(Frac::KEY_2) || m_inputSystem.GetJoystickComboOnce(m_inputSystem.GetDefaultJoystick(), Frac::JOYSTICK_BUTTON_LB, Frac::JOYSTICK_BUTTON_DPAD_RIGHT)) && camEnts.size() > 1)
	{
		SetMainCameraEntity(camEnts[1]);
	}
	else if ((m_inputSystem.GetKeyOnce(Frac::KEY_3) || m_inputSystem.GetJoystickComboOnce(m_inputSystem.GetDefaultJoystick(), Frac::JOYSTICK_BUTTON_LB, Frac::JOYSTICK_BUTTON_DPAD_DOWN)) && camEnts.size() > 2)
	{
		SetMainCameraEntity(camEnts[2]);
	}
	else if ((m_inputSystem.GetKeyOnce(Frac::KEY_4) || m_inputSystem.GetJoystickComboOnce(m_inputSystem.GetDefaultJoystick(), Frac::JOYSTICK_BUTTON_LB, Frac::JOYSTICK_BUTTON_DPAD_UP)) && camEnts.size() > 3)
	{
		SetMainCameraEntity(camEnts[3]);
	}
#endif
}

void CameraManager::UpdateDefenseGrid(Frac::EntityHandle e, TOR::Transform& transform, TOR::Camera& camera, CameraTrait& camTrait)
{
	const Frac::MouseState& mouse = m_inputSystem.GetMouse();
	
	const float horPan = m_inputSystem.GetAnyJoystickAxis(Frac::JoystickAxes::JOYSTICK_AXIS_LEFT_THUMB_HORIZONTAL);
	const float verPan = m_inputSystem.GetAnyJoystickAxis(Frac::JoystickAxes::JOYSTICK_AXIS_LEFT_THUMB_VERTICAL);
	
	const float zoom = (m_inputSystem.GetAnyJoystickAxis(Frac::JoystickAxes::JOYSTICK_AXIS_RIGHT_THUMB_VERTICAL)
						+ mouse.m_wheel) * camTrait.ZoomSpeed;

	const glm::mat4 lookAtMat = glm::lookAtRH(camTrait.LookAt, transform.Position, m_upVec);

	const glm::vec3 forward = glm::vec3(lookAtMat[0][2], lookAtMat[1][2], lookAtMat[2][2]);
	const glm::vec3 right	= glm::vec3(lookAtMat[0][0], lookAtMat[1][0], lookAtMat[2][0]);
	const glm::vec3 up		= glm::vec3(lookAtMat[0][1], lookAtMat[1][1], lookAtMat[2][1]);

	const glm::vec3 flatForward = glm::vec3(forward.x, 0.f, forward.z);
	const glm::vec3 flatMove = (right * -horPan) + (flatForward * -verPan);
	float dt = (camTrait.MoveSpeed * m_world.getDeltaTime());
	glm::vec3 mov = ( flatMove + forward * zoom) * dt;
	
	glm::vec3 newPos = transform.Position + mov;
	if (newPos.y > camTrait.MinMaxZoom.x && newPos.y < camTrait.MinMaxZoom.y)
	{
		camTrait.LookAt += glm::vec3(flatMove.x * dt, 0.f, flatMove.z * dt);
		transform.Position = newPos;
		transform.Orientation = glm::conjugate(glm::toQuat(glm::lookAt(transform.Position, camTrait.LookAt, { 0.f,1.f,0.f })));
	}
}

void CameraManager::UpdateNoclip(Frac::EntityHandle e, TOR::Transform& transform, TOR::Camera& camera, CameraTrait& camTrait)
{
	const float dt = m_world.getDeltaTime();
	
	const Frac::MouseState& mouse = m_inputSystem.GetMouse();
	
	const glm::vec2 mouseVel = ((mouse.m_position - mouse.m_lastPosition) * 1.5f) * dt;
	
	const float lHor = m_inputSystem.GetAnyJoystickAxis(Frac::JoystickAxes::JOYSTICK_AXIS_LEFT_THUMB_HORIZONTAL);
	const float lVer = m_inputSystem.GetAnyJoystickAxis(Frac::JoystickAxes::JOYSTICK_AXIS_LEFT_THUMB_VERTICAL);
	
	const float rHor = m_inputSystem.GetAnyJoystickAxis(Frac::JoystickAxes::JOYSTICK_AXIS_RIGHT_THUMB_HORIZONTAL)	+ mouseVel.x * (float)m_inputSystem.GetMouse().m_down[2];
	const float rVer = m_inputSystem.GetAnyJoystickAxis(Frac::JoystickAxes::JOYSTICK_AXIS_RIGHT_THUMB_VERTICAL)		+ mouseVel.y * (float)m_inputSystem.GetMouse().m_down[2];

	const bool multiplySpeed = m_inputSystem.GetAnyJoystickButton(Frac::JOYSTICK_BUTTON_L3);
	
	float upDown = 0.f;
	if (m_inputSystem.GetAnyJoystickButton(Frac::JOYSTICK_BUTTON_A))
	{
		upDown = 1.f;
	}
	else if (m_inputSystem.GetAnyJoystickButton(Frac::JOYSTICK_BUTTON_B))
	{
		upDown = -1.f;
	}

	const glm::mat4 lookAtMat = glm::lookAtRH(camTrait.LookAt, transform.Position, m_upVec);
	
	const glm::vec3 forward = glm::vec3(lookAtMat[0][2], lookAtMat[1][2], lookAtMat[2][2]);
	const glm::vec3 right	= glm::vec3(lookAtMat[0][0], lookAtMat[1][0], lookAtMat[2][0]);
	const glm::vec3 up		= glm::vec3(lookAtMat[0][1], lookAtMat[1][1], lookAtMat[2][1]);
	
	const float speedMultiplier = multiplySpeed ? camTrait.NoclipSpeedMultiplier : 1.f;
	
	transform.Position += ( ((right * -lHor) + (forward * -lVer) + (up * upDown)) * speedMultiplier ) * dt;
	
	camTrait.LookAt = transform.Position + glm::vec3(transform.Orientation * glm::vec3(0.f, 0.f, -1.f));

	transform.EulerOrientation += glm::vec3(
		glm::radians<float>((-rVer * camTrait.NoclipSpeed) * (speedMultiplier)), 
		glm::radians<float>((-rHor * camTrait.NoclipSpeed) * (speedMultiplier)), 
		0.f); // no roll??
	
	transform.Orientation = glm::quat(glm::vec3(transform.EulerOrientation));
}

void CameraManager::UpdateDemo(Frac::EntityHandle e, TOR::Transform& transform, TOR::Camera& camera, CameraTrait& camTrait)
{
	const float dt = m_world.getDeltaTime();

	camTrait.DemoTime += dt;
	
	transform.EulerOrientation += glm::vec3(
		glm::radians<float>(glm::sin(camTrait.DemoTime ) * dt), //* camTrait.DemoSpeed.x
		glm::radians<float>(camTrait.DemoSpeed.y * dt), 
		0.f);

	transform.Orientation = glm::quat(transform.EulerOrientation);
	transform.Position = (transform.Orientation * -camTrait.Offset) + camTrait.LookAt;
	transform.Orientation = glm::conjugate(glm::toQuat(glm::lookAt(transform.Position, camTrait.LookAt, { 0.f,1.f,0.f }))); // levi magic
}