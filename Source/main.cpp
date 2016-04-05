#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfxplatform.h>

#include <bgfx/bgfx.h>

#include "imgui_impl.h"

#include "drawing\field.h"
#include "drawing\robot.h"

SDL_Window* window;
uint32_t m_width = 1280;
uint32_t m_height = 720;
uint32_t m_debug = BGFX_DEBUG_TEXT;
uint32_t m_reset = BGFX_RESET_VSYNC;

void init()
{
	SDL_Init(0);
	window = SDL_CreateWindow("ImmView", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, 0);
	bgfx::sdlSetWindow(window);
	bgfx::init(bgfx::RendererType::Count);
	bgfx::reset(m_width, m_height, m_reset);

	// Enable debug text.
	bgfx::setDebug(m_debug);

	// Set view 0 clear state.
	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
		, 0x303030ff
		, 1.0f
		, 0
	);

	imguiInit(m_width, m_height);
}

void shutdown()
{
	imguiShutdown();

	// Shutdown bgfx.
	bgfx::shutdown();

	SDL_DestroyWindow(window);
	SDL_Quit();
}

void sendImInputMouse(float mousePos[2], int mouseButtons[3], float mouseWheel)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(mousePos[0], mousePos[1]);
	io.MouseDown[0] = mouseButtons[0] ? true : false;
	io.MouseDown[1] = mouseButtons[1] ? true : false;
	io.MouseDown[2] = mouseButtons[2] ? true : false;
	io.MouseWheel = mouseWheel;
}

void sendImInputChars(const char* chars)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddInputCharactersUTF8(chars);
}

void sendImInputKeys(const bool keysDown[512], bool shift, bool alt, bool ctrl)
{
	ImGuiIO& io = ImGui::GetIO();
	memcpy(io.KeysDown, keysDown, sizeof(io.KeysDown));
	io.KeyShift = shift;
	io.KeyAlt = alt;
	io.KeyCtrl = ctrl;
}

struct InputData
{
	int mouseButtons[3];
	float mouseWheel;
	bool keysDown[512];
	bool keyShift;
	bool keyCtrl;
	bool keyAlt;

	InputData()
	{
		mouseButtons[0] = mouseButtons[1] = mouseButtons[2] = 0;
		mouseWheel = 0.0f;
		keyShift = keyCtrl = keyAlt = false;
		memset(keysDown, 0x00, sizeof(keysDown));
	}
};

static InputData gInput;

bool sdlPollEvents()
{
	SDL_Event e;
	SDL_PollEvent(&e);

	switch (e.type) {
	case SDL_QUIT:
		return false;
	case SDL_MOUSEWHEEL:
	{
		if (e.wheel.y > 0)
			gInput.mouseWheel = 1.0f;
		else if (e.wheel.y < 0)
			gInput.mouseWheel = -1.0f;
		break;
	}

	case SDL_MOUSEBUTTONDOWN:
	{
		if (e.button.button == SDL_BUTTON_LEFT) gInput.mouseButtons[0] = 1;
		if (e.button.button == SDL_BUTTON_RIGHT) gInput.mouseButtons[1] = 1;
		if (e.button.button == SDL_BUTTON_MIDDLE) gInput.mouseButtons[2] = 1;
		break;
	}

	case SDL_TEXTINPUT:
		sendImInputChars(e.text.text);
		break;

	case SDL_KEYDOWN:
	case SDL_KEYUP:
	{
		int key = e.key.keysym.sym & ~SDLK_SCANCODE_MASK;
		gInput.keysDown[key] = e.type == SDL_KEYDOWN;
		gInput.keyShift = (SDL_GetModState() & KMOD_SHIFT) != 0;
		gInput.keyCtrl = (SDL_GetModState() & KMOD_CTRL) != 0;
		gInput.keyAlt = (SDL_GetModState() & KMOD_ALT) != 0;
		sendImInputKeys(gInput.keysDown, gInput.keyShift, gInput.keyAlt, gInput.keyCtrl);
		break;
	}

	default:
		break;
	}

	return true;
}

Field* field = new Field();
Robot* robot = new Robot();
float resize(ImVec2 wIdealSz) {
	ImVec2 winSz = ImGui::GetWindowSize();
	float min = winSz.x > winSz.y ? winSz.y : winSz.x;
	bool x = min == winSz.x ? true : false;
	ImVec2 ratio = ImVec2(winSz.x / wIdealSz.x, winSz.y / wIdealSz.y);
	return x ? (ratio.x > ratio.y ? ratio.x : ratio.y) : (ratio.x > ratio.y ? ratio.y : ratio.x);
}
void update()
{
	int mx, my;
	uint32_t mouseMask = SDL_GetMouseState(&mx, &my);

	float mousePos[2] = { 0.0f, 0.0f };
	if (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS) {
		mousePos[0] = (float)mx;
		mousePos[1] = (float)my;
	}
	else {
		mousePos[0] = -1.0f;
		mousePos[1] = -1.0f;
	}

	gInput.mouseButtons[0] = (mouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) ? 1 : 0;
	gInput.mouseButtons[1] = (mouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) ? 1 : 0;
	gInput.mouseButtons[2] = (mouseMask & SDL_BUTTON(SDL_BUTTON_MIDDLE)) ? 1 : 0;

	sendImInputMouse(mousePos, gInput.mouseButtons, gInput.mouseWheel);

	gInput.mouseButtons[0] = gInput.mouseButtons[1] = gInput.mouseButtons[2] = 0;
	gInput.mouseWheel = 0.0f;

	// Set view 0 default viewport.
	bgfx::setViewRect(0, 0, 0, m_width, m_height);

	// This dummy draw call is here to make sure that view 0 is cleared
	// if no other draw calls are submitted to view 0.
	bgfx::touch(0);
	
	imguiNewFrame();
	bool opened = true;
	ImVec2 margin = ImVec2(20,20)*2;
	ImVec2 wSize = ImVec2(680.0f, 460.0f)+margin;
	// TODO: draw gui
	ImGui::SetNextWindowSize(wSize, ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Field", &opened))
	{
		ImGui::End();
		return;
	}
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	float zoom = 1.0f;// resize(wSize);
	{
		field->draw(draw_list, zoom);
		robot->draw(draw_list, zoom);
	}
	ImGui::End();
	imguiRender();

	// Use debug font to print information about this example.
	bgfx::dbgTextClear();
	/*bgfx::dbgTextImage(bx::uint16_max(m_width / 2 / 8, 20) - 20
		, bx::uint16_max(m_height / 2 / 16, 6) - 6
		, 40
		, 12
		, s_logo
		, 160
	);*/
	bgfx::dbgTextPrintf(0, 1, 0x4f, "bgfx/examples/00-helloworld");
	bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Initialization and debug text.");

	// Advance to next frame. Rendering thread will be kicked to
	// process submitted rendering primitives.
	bgfx::frame();

}

int main(int argc, char *argv[])
{
	init();

	bgfx::frame();

	while (sdlPollEvents())
		update();

	shutdown();

	return 0;
}