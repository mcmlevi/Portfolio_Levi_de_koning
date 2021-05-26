#pragma once

namespace Frac
{
	/**
	* The ITools is the base class for Platform specific tool implementations
	*/
	class ITools
	{
	public:
		ITools() = default;
		virtual ~ITools() = default;

		/**
		* Updates all the platform specific tools
		*/
		virtual void Update() = 0;


	};
} // namespace Frac