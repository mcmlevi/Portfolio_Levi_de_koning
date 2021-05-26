#pragma once


namespace Frac
{
	/**
	* The IDevice is the base class for Platform specific window implementations
	*/
	class IDevice
	{
	public:
		IDevice() = default;
		virtual ~IDevice() = default;

		virtual void Initialize(unsigned width, unsigned height) = 0;

		/**
		* This function Swaps the front and back buffers
		*/
		virtual void SwapBuffers() = 0;

		/**
		* Event polling needs to be done in order to receive monitor and joystick connection events
		* This should be done after swapping the buffers
		*/
		virtual void PollEvents() = 0;

		/**
		* Checks whether the user is requesting the window to be closed
		* @return Close state
		*/
		virtual bool ShouldClose() = 0;

		virtual unsigned GetWindowWidth() const = 0;
		virtual unsigned GetWindowHeight() const = 0;
	};
} // namespace Frac