#include "SFMLWrapper.h"
#include "cassert"
#include "string.h"
#include "Camera.h"
#include "Ray.h"
#include "e_scenes.h"
#include "magic_enum/include/magic_enum.hpp"
#pragma warning(push)
#pragma warning(disable : 26451)
#pragma warning(disable : 26812)
constexpr int channels{ 4 };
SFMLWrapper::SFMLWrapper(const int width, const int height) :m_width(width), m_height(height)
{
	m_highResHeight = sf::VideoMode::getDesktopMode().height;
	m_highResWidth = sf::VideoMode::getDesktopMode().width;
	window = new sf::RenderWindow(sf::VideoMode(static_cast<unsigned>(width), static_cast<unsigned>(height)), "Levi's amazing ray tracer!");
	pixels = new sf::Uint8[width * height * channels]{};
	highResPixels = new sf::Uint8[m_highResWidth * m_highResHeight * channels]{};
	pixelBuffer = new float[m_highResWidth *m_highResHeight * channels]{};
	//reducedPixels = new sf::Uint8[width * height * 4]{};
	m_image.create(width, height);
	m_highResImage.create(m_highResWidth, m_highResHeight);
	
	m_sprite = sf::Sprite{ m_image };
	m_highResSprite = sf::Sprite{ m_highResImage };
	m_highResSprite.scale({ (float)width / (float) m_highResWidth,(float)height / (float)m_highResHeight });
	if (defaultFont.loadFromFile("assets/Arial.ttf") == false)
	{
		assert(false && "File not loaded.");
	}
	helpText = { "Press F1 to Toggle help. Press F2 for Stats.",defaultFont,10 };

	infoTexts[0] = { "W A S D to move forward, backward, left and right.",defaultFont,10 };

	infoTexts[1] = { "Q and E to move up and down", defaultFont,10 };

	infoTexts[2] = { "Num 8,4,5,6 to rotate left, right, up and down.",defaultFont,10 };

	infoTexts[3] = { "Num 7 and 8 to tilt the camera." ,defaultFont,10 };

	infoTexts[4] = { "Press C to take a screenshot (RayTracer/screenshots). ",defaultFont,10 };

	infoTexts[5] = { "Press page up and page down to change the FOV.", defaultFont,10 };
	infoTexts[6] = { "Press + and - to change movement Speed.", defaultFont,10 };
	infoTexts[7] = { "Press numpad1 and numpad2 to change the max depth.", defaultFont,10 };
	infoTexts[8] = { "Press N to visualize the normals.", defaultFont,10 };
	infoTexts[9] = { "Press M to visualize the BVH.", defaultFont,10 };
	infoTexts[10] = { "Press F5 and F6 to switch scenes.", defaultFont,10 };
	infoTexts[11] = { "Press F9 and F10 to change BVH view Depth." , defaultFont, 10};
	infoTexts[12] = { "Press V for screenShot mode." , defaultFont, 10 };
	infoTexts[13] = { "Press up or down to change focal lenght." , defaultFont, 10 };
	for (int i = 0; i < numOfInfoTextElements; ++i)
		infoTexts[i].move(0, i * 10.f + 10.f);
	
	for (int i = 0; i < numOfStatsTextElements; ++i)
	{
		statTexts[i] = { "",defaultFont,10 };
		statTexts[i].move(300, 10.f * i + 10.f);
	}

}

SFMLWrapper::~SFMLWrapper()
{
	delete window;
	window = nullptr;

	delete window;
	window = nullptr;

	delete[] pixels;
	delete[] pixelBuffer;
}



void SFMLWrapper::updateWindow(bool highResImage)
{
	window->clear();

	if (highResImage == false)
		window->draw(m_sprite);
	else
	{
		//window->draw(m_sprite);
		//m_highResSprite.setScale({ (float)m_highResWidth,(float)m_highResHeight });
		window->draw(m_highResSprite);
	}

	updateUI();
	window->display();
}

void SFMLWrapper::updateImage()
{
	m_image.update(pixels);
	m_highResImage.update(highResPixels);
}

void SFMLWrapper::lockWindow()
{
	sf::Event event;
	while (window->isOpen())
	{
		window->pollEvent(event);
		if (event.type == sf::Event::Closed)
		{
			window->close();
		}
		window->clear();
		window->draw(m_sprite);
		window->display();
	}
}

void SFMLWrapper::setStats(float currentFrameTime, Camera& camera, float movementSpeed, int maxDepth, bool visualizeNormals, Scenes currentScene, bool visualizeBVH, int BVH_visualizationLayer, bool screenShotMode, float focalLenght, float apature)
{
	
	
	Ray cameraInfo{ camera.getRayAt(m_height / 2, m_width / 2) };
	statTexts[0].setString("Screen width: " + (screenShotMode == true?std::to_string(m_highResWidth): std::to_string(m_width)));
	statTexts[1].setString("Screen Height: " + (screenShotMode ==true ? std::to_string(m_highResHeight) : std::to_string(m_height)));
	statTexts[2].setString("Current Frame time: " + std::to_string(currentFrameTime));
	statTexts[3].setString("Current FPS : " + std::to_string(1000.f / currentFrameTime));
	statTexts[4].setString("Camera Fov: " + std::to_string(camera.getFOV()));
	statTexts[5].setString("Camera Location:  X: " + std::to_string(cameraInfo.m_orgin.m_x) + " Y: " + std::to_string(cameraInfo.m_orgin.m_y) + " Z: " + std::to_string(cameraInfo.m_orgin.m_z));
	statTexts[6].setString("Camera Looking Direction:  X: " + std::to_string(cameraInfo.m_direction.m_x) + " Y: " + std::to_string(cameraInfo.m_direction.m_y) + " Z: " + std::to_string(cameraInfo.m_direction.m_z));
	statTexts[7].setString("MovementSpeed: " + std::to_string(static_cast<int>(round(movementSpeed * 100.f))) + "%");
	statTexts[8].setString( "Current Recursion depth: " + std::to_string(maxDepth));
	statTexts[9].setString("Vizualize normal Vectors: " + (visualizeNormals ? std::string{ "True" } : std::string{ "False" }));
	statTexts[10].setString("Vizualize BVH: " + (visualizeBVH ? std::string{ "True" } : std::string{ "False" }) + " Layer: "+ std::to_string(BVH_visualizationLayer));
	statTexts[11].setString("Current Scene: " + std::string{ magic_enum::enum_name(currentScene) });
	statTexts[12].setString("Screenshot mode: " + (screenShotMode ? std::string{ "True" } : std::string{ "False" }) + " Focal Lenght: " + std::to_string(-focalLenght) + " Apature: "+std::to_string(apature));
}

void SFMLWrapper::updateUI()
{
	window->draw(helpText);
	if (showInfoText)
	{
		for (int i = 0; i < numOfInfoTextElements; ++i)
		{
			window->draw(infoTexts[i]);
		}
	}
	if (showStats)
	{
		for (int i = 0; i < numOfStatsTextElements; ++i)
		{
			window->draw(statTexts[i]);
		}
	}
}

//void SFMLWrapper::antiAlias()
//{
//	for (int y = 0; y < m_height; y += m_superSamplingFactor)
//	{
//		for (int x = 0; x < m_width; x += m_superSamplingFactor)
//		{
//			unsigned int R{};
//			unsigned int G{};
//			unsigned int B{};
//			unsigned int A{};
//			for (int sY = 0; sY < m_superSamplingFactor; ++sY)
//			{
//				for (int sX = 0; sX < m_superSamplingFactor; ++sX)
//				{
//					R += (unsigned)pixels[(y + sY) * m_width * 4 + (x + sX) * 4];
//					G += (unsigned)pixels[(y + sY) * m_width * 4 + (x + sX) * 4 + 1];
//					B += (unsigned)pixels[(y + sY) * m_width * 4 + (x + sX) * 4 + 2];
//					A += (unsigned)pixels[(y + sY) * m_width * 4 + (x + sX) * 4 + 3];
//				}
//			}
//			assert(y % m_superSamplingFactor == 0);
//			assert(x % m_superSamplingFactor == 0);
//			assert(m_width % m_superSamplingFactor == 0);
//			reducedPixels[(y / m_superSamplingFactor) * (m_width / m_superSamplingFactor) * 4 + (x / m_superSamplingFactor) * 4 + 0] = R / m_superSamplingFactor;
//			reducedPixels[(y / m_superSamplingFactor) * (m_width / m_superSamplingFactor) * 4 + (x / m_superSamplingFactor) * 4 + 1] = G / m_superSamplingFactor;
//			reducedPixels[(y / m_superSamplingFactor) * (m_width / m_superSamplingFactor) * 4 + (x / m_superSamplingFactor) * 4 + 2] = B / m_superSamplingFactor;
//			reducedPixels[(y / m_superSamplingFactor) * (m_width / m_superSamplingFactor) * 4 + (x / m_superSamplingFactor) * 4 + 3] = A / m_superSamplingFactor;
//		}
//	}
//}
//
#pragma  warning(pop)