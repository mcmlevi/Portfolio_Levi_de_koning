#pragma once
#include <memory>
#include "Core/EngineConnector.h"

namespace Frac {
	/// <summary>
	/// The frame listener interface provides a base class for games to inherit from so they can listen to frame start and end events.
	/// </summary>
	class FrameListener {
	public:

		FrameListener() = default;
		virtual ~FrameListener() = default;

		FrameListener(const FrameListener&) = delete;
		FrameListener& operator=(const FrameListener&) = delete;

		/// <summary>
		/// Called at the start of a new frame.
		/// </summary>
		virtual void FrameStarted() = 0;

		/// <summary>
		/// Called after the scene is rendered to the screen.
		/// </summary>
		virtual void FrameEnded() = 0;

		std::unique_ptr<EngineConnector>& GetEngineConnector();

	private:

		std::unique_ptr<EngineConnector> m_engineConnector;
	};
}