#pragma once

namespace Frac {

	class EngineCore;

	class EngineConnector {
	public:
		EngineConnector() {};
		~EngineConnector() {};


		EngineCore& GetEngine();
	};
}