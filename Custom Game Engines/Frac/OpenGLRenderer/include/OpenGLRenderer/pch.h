#pragma once
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <any>
#include <set>
#include <queue>
#include <cstdio>
#include <map>
#include <unordered_map>
#include <spdlog/spdlog.h>
#include <sstream>
#include <regex>

// string utility
#include <iomanip>
#include <iosfwd>
#include <fstream>

#if defined (PLATFORM_WINDOWS)
#include <glad/glad.h>
#elif defined (PLATFORM_SWITCH)

#endif
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.inl>

#include "OpenGLRenderer/Utility.h"

#if defined(PLATFORM_SWITCH)

#elif defined(PLATFORM_WINDOWS)
#include <cassert>
#define TOR_ASSERT assert
#endif