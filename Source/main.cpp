#include <SDL.h>
#include <SDL_syswm.h>
#include <bgfx/bgfxplatform.h>

#include <bgfx/bgfx.h>

#include "imgui_impl.h"
#include <list>
#include <thread>
#include <atomic>

#include "protos/messages_robocup_ssl_wrapper.pb.h"
#include "drawing/FieldRenderer.h"

#include "utility/network/robocup_ssl_client.h"

#include "protos/messages_immortals_world_state.pb.h"

#include <zmq.h>

SDL_Window* window;
int m_width = 1600;
int m_height = 900;
uint32_t m_debug = BGFX_DEBUG_TEXT;
uint32_t m_reset = BGFX_RESET_VSYNC;

RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize* ssl_field;
SSL_WrapperPacket* ssl_packet;
SSL_WrapperPacket* ssl_packet_off;
FieldRenderer* field_renderer;
RoboCupSSLClient* sslClient;
Immortals::Data::WorldState* world_state;
Immortals::Data::WorldState* world_state_off;

mutex vision_mutex;
mutex reality_mutex;

void init()
{
	SDL_Init(0);
	window = SDL_CreateWindow("ImmortalsDBG", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_width, m_height, SDL_WINDOW_RESIZABLE);
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
	// Setup display size (every frame to accommodate for window resizing)
	ImGuiIO& io = ImGui::GetIO();


	int w, h;
	SDL_GetWindowSize(window, &w, &h);
	if (w != m_width || h != m_height) {
		m_width = w;
		m_height = h;
		int display_w, display_h;
		SDL_GL_GetDrawableSize(window, &display_w, &display_h);
		io.DisplaySize = ImVec2((float) m_width, (float) m_height);
		io.DisplayFramebufferScale = ImVec2(m_width > 0 ? ((float) display_w / m_width) : 0,
											m_height > 0 ? ((float) display_h / m_height) : 0);
		bgfx::reset(m_width, m_height, m_reset);
	}
	bgfx::setViewRect(0, 0, 0, m_width, m_height);
	// This dummy draw call is here to make sure that view 0 is cleared
	// if no other draw calls are submitted to view 0.
	bgfx::touch(0);
	
	imguiNewFrame();
	static bool opened = true;
	ImVec2 margin = ImVec2(30,30)*2;
	ImVec2 wSize = ImVec2(900.f, 600.f)+margin;
	// TODO: draw gui
	ImGui::SetNextWindowSize(wSize, ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Field", &opened))
	{
		ImGui::End();
	}
	else {
		ImDrawList *draw_list = ImGui::GetWindowDrawList();

		field_renderer->SetWidgetProperties(ImGui::GetWindowPos() + ImGui::GetCursorPos(), ImGui::GetWindowSize() - ImGui::GetCursorPos() * 2.f);
		field_renderer->SetDrawList(draw_list);
		field_renderer->DrawFieldLegacy(*ssl_field);

		vision_mutex.lock();
		field_renderer->DrawBalls(ssl_packet->detection().balls());
		field_renderer->DrawRobots(ssl_packet->detection().robots_blue(), Blue);
		field_renderer->DrawRobots(ssl_packet->detection().robots_yellow(), Yellow);
		vision_mutex.unlock();
		reality_mutex.lock();
		field_renderer->DrawBalls(world_state->balls());
		reality_mutex.unlock();

		ImGui::End();
	}

	imguiRender();

	// Advance to next frame. Rendering thread will be kicked to
	// process submitted rendering primitives.
	bgfx::frame();

}

void receiveVision()
{

}

int main(int argc, char *argv[])
{
	init();

	bgfx::frame();
	
	field_renderer = new FieldRenderer();

	ssl_field = new RoboCup2014Legacy::Geometry::SSL_GeometryFieldSize();
	ssl_field->set_field_length(9000);
	ssl_field->set_field_width(6000);
	ssl_field->set_boundary_width(700);
	ssl_field->set_referee_width(300);
	ssl_field->set_center_circle_radius(500);
	ssl_field->set_defense_radius(1000);
	ssl_field->set_defense_stretch(500);
	ssl_field->set_goal_width(1000);
	ssl_field->set_goal_depth(180);

	field_renderer->SetFieldSizeLegacy(*ssl_field);

	ssl_packet_off = new SSL_WrapperPacket();
	ssl_packet = new SSL_WrapperPacket();
	auto ball = ssl_packet->mutable_detection()->add_balls();
	ball->set_x(0.f);
	ball->set_y(0.f);
	

	auto robot = ssl_packet->mutable_detection()->add_robots_blue();
	robot->set_x(1500);
	robot->set_confidence(0.7);
	robot->set_orientation(1.5);
	robot = ssl_packet->mutable_detection()->add_robots_yellow();
	robot->set_y(2460);
	robot->set_confidence(0.95);

	sslClient = new RoboCupSSLClient();
	sslClient->open(true);

	atomic<bool> running(true);

	auto vision_func = [&]()
	{
		map<uint32_t, SSL_DetectionFrame> frame_map;
		SSL_WrapperPacket tmp_ssl_packet;

		while (running) {
			sslClient->receive(tmp_ssl_packet);

			if (tmp_ssl_packet.has_detection())
			{
				auto detection = tmp_ssl_packet.detection();
				frame_map[detection.camera_id()] = detection;
			}

			ssl_packet_off->clear_detection();
			for(auto detection: frame_map)
			{
				ssl_packet_off->mutable_detection()->MergeFrom(detection.second);
			}

			vision_mutex.lock();
			swap(ssl_packet, ssl_packet_off);
			vision_mutex.unlock();
		}
	};

	world_state = new Immortals::Data::WorldState();
	world_state_off = new Immortals::Data::WorldState();

	auto zmq_fun = [&]()
	{
		//  Socket to talk to server
		printf("Collecting updates from ai server\n");
		void *context = zmq_ctx_new();
		void *subscriber = zmq_socket(context, ZMQ_SUB);
		int rc = zmq_connect(subscriber, "tcp://localhost:5556");
		assert(rc == 0);

		//  Subscribe to zipcode, default is NYC, 10001
		const char *filter = "";
		rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE,
			filter, strlen(filter));
		assert(rc == 0);

		const int buffer_size = 1000*1000;
		char* buffer = new char[buffer_size];


		while (running)
		{
			this_thread::sleep_for(chrono::milliseconds(1));
			int received_size = zmq_recv(subscriber, buffer, buffer_size, 0);

			printf("Received ai debug blob of size %d\n", received_size);

			if (!world_state_off->ParseFromArray(buffer, received_size))
				continue;

			reality_mutex.lock();
			swap(world_state, world_state_off);
			reality_mutex.unlock();
			//printf("WorldState (%.2f, %.2f)\n", world_state.ball().position().x(), world_state.ball().position().y());
		}
		zmq_close(subscriber);
		zmq_ctx_destroy(context);

		delete buffer;
	};

	thread vision_thread(vision_func);
	thread zmq_thread(zmq_fun);

	while (sdlPollEvents())
	{
		update();
	}

	running = false;
	shutdown();
	vision_thread.join();
	zmq_thread.join();
	return 0;
}