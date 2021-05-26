#pragma once
#include <string>
#include "Glm/gtx/transform.hpp"
struct LevelData
{
public:
	int m_levelId;
	std::string m_levelName;
	int m_levelTargetScore;
	int m_levelWaves;
	glm::vec3 m_cameraStartPosition;
	glm::vec3 m_cameraStartRotation;
private:
};