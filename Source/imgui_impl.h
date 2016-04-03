#pragma once

#include "../3rdparty/imgui/imgui.h"

enum class gfxGuiKeyMap : int
{
	Tab,       // for tabbing through fields
	LeftArrow, // for text edit
	RightArrow,// for text edit
	UpArrow,   // for text edit
	DownArrow, // for text edit
	PageUp,
	PageDown,
	Home,      // for text edit
	End,       // for text edit
	Delete,    // for text edit
	Backspace, // for text edit
	Enter,     // for text edit
	Escape,    // for text edit
	A,         // for text edit CTRL+A: select all
	C,         // for text edit CTRL+C: copy
	V,         // for text edit CTRL+V: paste
	X,         // for text edit CTRL+X: cut
	Y,         // for text edit CTRL+Y: redo
	Z,         // for text edit CTRL+Z: undo

	Count
};

int imguiInit(uint16_t viewWidth, uint16_t viewHeight);
void imguiShutdown();

void imguiRender();
void imguiNewFrame();
