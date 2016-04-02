#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfxplatform.h>

#include <bgfx/bgfx.h>

SDL_Window* window;
uint32_t m_width = 1280;
uint32_t m_height = 720;
uint32_t m_debug = BGFX_DEBUG_TEXT;
uint32_t m_reset = BGFX_RESET_VSYNC;

void init()
{
	window = SDL_CreateWindow("ImmView", 0, 0, m_width, m_height, 0);
	bgfx::sdlSetWindow(window);

	bgfx::init(bgfx::RendererType::Direct3D11);
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
}

void shutdown()
{
	// Shutdown bgfx.
	bgfx::shutdown();

	SDL_DestroyWindow(window);
}

bool update()
{
	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT) {
			quit = true;
		}
	}

	if (!quit)
	{
		// Set view 0 default viewport.
		bgfx::setViewRect(0, 0, 0, m_width, m_height);

		// This dummy draw call is here to make sure that view 0 is cleared
		// if no other draw calls are submitted to view 0.
		bgfx::touch(0);

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

		return true;
	}

	return false;
}

int main(int argc, char *argv[])
{
	init();

	bgfx::frame();

	while (update());

	shutdown();

	return 0;
}