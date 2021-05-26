#pragma once
#include <SFML/Graphics.hpp>

#include "vec3f.h"

enum class Scenes;
class Camera;

class SFMLWrapper
{
public:
	SFMLWrapper(const int width, const int height);
	~SFMLWrapper();

	int m_width;
	int m_height;
	unsigned m_highResWidth{ 1920 };
	unsigned m_highResHeight{ 1080 };
	void updateWindow(bool highResImage);
	void updateImage();
	void lockWindow();
	sf::Uint8* pixels;
	sf::Uint8* highResPixels;
	float* pixelBuffer;
	sf::RenderWindow& getWindow() { return  *window; }
	//void antiAlias();
	bool showInfoText{ false };
	bool showStats{ false };
	void setStats(float currentFrameTime, Camera& camera, float movementSpeed, int maxDepth, bool visualizeNormals, Scenes currentScene, bool visualizeBVH, int BVHVisualizationLayer, bool screenShotMode,float focalLenght, float apature);
private:
	void updateUI();
	//int m_superSamplingFactor;
	//sf::Uint8* reducedPixels;
	sf::RenderWindow* window;
	sf::Texture m_image;
	sf::Texture m_highResImage;
	sf::Sprite m_sprite;
	sf::Sprite m_highResSprite;
	sf::Font defaultFont;

	sf::Text helpText;
	static const int numOfInfoTextElements{ 14 };
	static const int numOfStatsTextElements{ 13 };
	sf::Text infoTexts[numOfInfoTextElements];
	sf::Text statTexts[numOfStatsTextElements];
};

