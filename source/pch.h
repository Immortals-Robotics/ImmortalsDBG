#pragma once

#include <string>
#include <mutex>
#include <span>
#include <list>
#include <thread>
#include <atomic>

#if defined(_WIN32)           
	#define NOGDI             // All GDI defines and routines
	#define NOUSER            // All USER defines and routines
#endif

#include <asio.hpp>

#if defined(_WIN32)           // raylib uses these names as function parameters
	#undef near
	#undef far
#endif

#include <raylib.h>
#include <raymath.h>

#include <imgui.h>
#include <rlimgui/rlImGui.h>

#include <homog2d.hpp>
using namespace h2d;
