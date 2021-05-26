#pragma once
#include "Graphics/Components/TextComponent.h"

#include "Core\EngineCore.h"

#include "EGameState.h"
#include "MouseCursorManager.h"

class GameManager;

class UIManager
{
public:
	struct UIPopUp
	{
		Frac::EntityHandle theEnt;
		std::unordered_map<std::string, Frac::EntityHandle> infoElements;
	};
	
	struct UIElement
	{
		std::string eventName = ""; /*! <- used when this element should switch active canvas */
	};
	
	struct UICanvas
	{
		std::string name = "";

		// Settable (resource counter, etc)
		std::unordered_map<std::string, Frac::EntityHandle> settableElements;
		
		// Selection (menu items, buttons, etc)
		Frac::EntityHandle theSelectorEnt = 0; /*<- if a selector is present that means we can select stuff*/
		std::vector<Frac::EntityHandle> selectableElements;
		int curSelectedElement = 0;
		int lastSelectedElement = 0;

		glm::vec3 idleColor = glm::vec3(1.f);
		glm::vec3 hoverColor = glm::vec3(0.25f, 1.f, 0.25f);

		float selectorAnimTime = 0.f;
		float selectorAnimSpeed = 8.f;
		float selectorAnimBounce = 4.f;
		float selectorAnimCurrent = 0.f;
		
		bool isActive = true;
	};

	UIManager(Frac::EngineCore& engine, GameManager& gameManager);
	~UIManager() = default;

	void Initialise(GameState*);
	void Update();
	
	std::string CreateMainMenuCanvas();
	std::string CreateGameplayCanvas();
	std::string CreatePauseCanvas();
	std::string CreateVictoryCanvas();
	std::string CreateDefeatCanvas();

	std::string CreateControlsCanvas();
	std::string CreateOptionsCanvas();

	/*!
	 * @brief Handles the popup entity
	 */
	void HandlePopupEntity(Frac::EntityHandle target);
	
private:
	void SetInitialActiveCanvas();

	void UpdateCanvases(EntityHandle entity, TOR::Transform& transform, UICanvas& canvas, Frac::TextComponent& text);

	void UpdateMainMenuCanvas	(EntityHandle entity, UICanvas& canvas);
	void UpdateGameplayCanvas	(EntityHandle entity, UICanvas& canvas);
	void UpdatePauseCanvas		(EntityHandle entity, UICanvas& canvas);
	void UpdateControlsCanvas	(EntityHandle entity, UICanvas& canvas);
	void UpdateOptionsCanvas	(EntityHandle entity, UICanvas& canvas);
	void UpdateVictoryCanvas	(EntityHandle entity, UICanvas& canvas);
	void UpdateDefeatCanvas	(EntityHandle entity, UICanvas& canvas);
	
	void UpdateCanvasSelector(UICanvas& canvas);

	/*!
	 * @brief always makes sure canvases are following and facing the camera
	 */
	void UpdateCanvasTransform(TOR::Transform& transform, Frac::TextComponent& text);
	/*!
	* @brief A centralized place to handle input from
	*/
	void HandleInput();
	
	void SetActiveCanvas(Frac::EntityHandle toDeactive, Frac::EntityHandle toActive);
	void SetGameState(const std::string& stringToCheck);
	
	Frac::EntityHandle GetActiveCamera(); /*<- is query*/

	std::unordered_map<std::string, Frac::EntityHandle> m_canvasesMap; 
	
	Frac::EntityHandle m_lastActiveCanvasEnt;
	Frac::EntityHandle m_activePopupEnt;

	GameState* m_gameState;

	Frac::EngineCore& m_engine;
	Frac::ECSWrapper& m_world;
	Frac::InputSystem& m_inputSystem;

	std::vector<std::pair<std::string, std::string>> m_menuItems; /* <- first = query id, second = button text */

	Frac::Query<TOR::Camera> m_camEntsQuery;
	Frac::System<TOR::Transform, UICanvas, Frac::TextComponent> m_updateCanvasSys;

	std::unique_ptr<MouseCursorManager> m_cursor3DManager;

	Frac::EntityHandle m_activeCamera;
	
	bool m_ignoreFirst; /*! <- When active canvas is updated, make sure that next canvas update is skipped so the InputSystem can properly update. Else GetKeyOnce don not function as intended */
	bool m_initialRun; /*! <- redo states at initial run */
	
	GameManager& m_gameManager; /*! <- Used for getting resources */

	// INPUT
	bool m_select;
	bool m_up;
	bool m_down;
	bool m_pause;
	
protected:
	const float m_canvasDepthOffset		= 200.f;

	const glm::vec3 m_logoPos			= glm::vec3(0.f, 80.f, 0.f);
	const glm::vec3 m_logoSize			= glm::vec3(0.2f);
	
	const glm::vec3 m_backdropPos		= glm::vec3(0.f, -40.f, 0.f);
	const glm::vec3 m_backdropSize		= glm::vec3(1.0f, 1.0f, 1.f);
	const glm::vec3 m_backdropColor		= glm::vec3(0.f);
	const float m_backdropFontSize		= 4.0f;

	const float m_buttonDepth			= -1.0f;
	const float m_buttonVerStartOffset	= 45.f;
	const float m_buttonVerPadding		= 30.f;
	const float m_buttonFontSize		= 0.75f;

	// options, controls related
	const float m_controlsButtonVerOffset	= -60.f;

	// game play canvas related
	const glm::vec3 m_iconBasePos		= glm::vec3(220.f, 120.f, 0.f);
	
	const float m_iconHorOffset			= -60.f;
	const glm::vec3 m_resCountColor		= glm::vec3(1.f, 1.f, 1.f);

	const glm::vec3 m_counterListOffset	= glm::vec3(0.f, -30.f, 0.f);
	
	const glm::vec3 m_resCountPos		= glm::vec3(0.f, 0.f, 0.f);
	const glm::vec3 m_waveCounterPos	= glm::vec3(0.f, 40.f, 0.f);
	const glm::vec3 m_incomingCounterPos= glm::vec3(0.f, 80.f, 0.f);
	const glm::vec3 m_baseHpCounterPos	= glm::vec3(0.f, 120.f, 0.f);
	
	// win/lose
	const glm::vec3 m_winBackDropCol	= glm::vec3(0.3f, 0.5f, 0.3f);
	const glm::vec3 m_winHeaderCol		= glm::vec3(0.3f, 1.f, 0.3f);
	
	const glm::vec3 m_loseBackDropCol	= glm::vec3(0.75f, 0.4f, 0.4f);
	const glm::vec3 m_loseHeaderCol		= glm::vec3(1.f, 0.4f, 0.4f);

	const float m_winLoseBackdropSize	= 3.f;
	const float m_winLoseButtonVerOffset= -10.f;

	// popup
	const glm::vec3 m_popupCol			= glm::vec3(0.f, 0.f, 0.f);
	const glm::vec3 m_popupSize			= glm::vec3(0.045f, 0.05f, 0.05f);

	const glm::vec3 m_popupOffset		= glm::vec3(-1.f, 1.5f, 0.5f);
};