#pragma once

#include <cstddef>			// size_t
#include <memory>
#include <algorithm>
#include <chrono>

#if defined(PLATFORM_SWITCH)

#elif defined(PLATFORM_WINDOWS)
#include <cassert>
#define FRAC_ASSERT assert
#endif
#include <cstdint>			//uint_64

// IO
#include <sstream>
#include <ostream>
#include <iostream>
#include <cstdio>

// Containers
#include <vector>
#include <array>
#include <unordered_map>
#include <string>
#include <queue>
#include <stack>
#include <list>

// Externals
#include <Glm/glm.hpp>
#include <Glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <CoreRenderAPI/Components/RenderComponents.h>

// Utility
#include "Logger.h"