#pragma once
#include <cstdint>
namespace Frac
{
	class ECSWrapper;
	class FrameListener;
typedef uint64_t FrameListenerHandle;
class FrameListenerManager
{
public:
	void Initialize(ECSWrapper& wrapper);
	FrameListenerHandle AddFrameListener(FrameListener* listener);
private:
};
}// Namespace Frac

