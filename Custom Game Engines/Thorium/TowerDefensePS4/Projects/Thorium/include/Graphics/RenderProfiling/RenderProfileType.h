#pragma once

/// <summary>
/// Used to select the render detail that the renderer uses. Also applicable when profiling/benchmarking the renderer.
/// </summary>
enum class ERenderProfileType
{
	low = 0,
	medium,
	high,
	ultra
};