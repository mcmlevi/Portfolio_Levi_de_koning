#pragma once
#include "AABB.h"
struct JBVNode
{
	JBVNode() = default;
	~JBVNode() = default;
	AABBox boundingBox{};
	int left{};
	int count{};
};


