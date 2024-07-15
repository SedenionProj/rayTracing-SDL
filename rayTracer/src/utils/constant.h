#pragma once
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <limits>

#define PI glm::pi<float>()
#define ONE_MINUS_EPSILON float(0x1.fffffep-1)

constexpr uint32_t MAX_UINT32 = std::numeric_limits<uint32_t>::max();
constexpr int32_t MAX_INT32 = std::numeric_limits<int32_t>::max();
constexpr int16_t MAX_INT16 = std::numeric_limits<int16_t>::max();
constexpr float MAX_FLOAT = std::numeric_limits<float>::max();