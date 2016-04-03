#include "bx/fpumath.h"
#include "bx/uint32_t.h"

#define BX_IMPLEMENT_LOGGER
#include "bxx/logger.h"

#include <bgfx/bgfx.h>

#include "imgui_impl.h"

#include <bx/allocator.h>

#define STB_LEAKCHECK_IMPLEMENTATION
#include <bxx/leakcheck_allocator.h>
#include <SDL.h>

#if BX_PLATFORM_WINDOWS
#   include "shaders/build/Windows/fs_imgui.h"
#   include "shaders/build/Windows/vs_imgui.h"
#endif

#define MAX_VERTICES 5000
#define MAX_INDICES 10000

#ifdef _DEBUG
static bx::LeakCheckAllocator gAlloc;
#else
static bx::CrtAllocator gAlloc;
#endif

#define IS_VALID(_Handle) (_Handle.idx != bgfx::invalidHandle)

struct ImGuiImpl
{
    bx::AllocatorI* alloc;
    bgfx::ProgramHandle progHandle;
	bgfx::DynamicVertexBufferHandle vertexBuffer;
	bgfx::DynamicIndexBufferHandle indexBuffer;
    
	bgfx::TextureHandle fontTexHandle;
	bgfx::UniformHandle uniformTexture;

    ImGuiImpl()
    {
        fontTexHandle = BGFX_INVALID_HANDLE;
        progHandle = BGFX_INVALID_HANDLE;
        vertexBuffer = BGFX_INVALID_HANDLE;
        uniformTexture = BGFX_INVALID_HANDLE;
        alloc = nullptr;
    }
};

struct VertexPosCoordColor
{
    float x;
    float y;
    float tx;
    float ty;
    uint32_t color;
    
    static void init()
    {
        Decl.begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    }
    static bgfx::VertexDecl Decl;
};
bgfx::VertexDecl VertexPosCoordColor::Decl;

static ImGuiImpl* gIm = nullptr;

static void* imguiAlloc(size_t size)
{
    return BX_ALLOC(gIm->alloc, size);
}

static void imguiFree(void* ptr)
{
    if (gIm)
        BX_FREE(gIm->alloc, ptr);
}

static void imguiDrawLists(ImDrawData* data)
{
    assert(gIm);

    float proj[16];
    bx::mtxOrtho(proj, 0.0f, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, 0.0f, -1.0f, 1.0f);
    bgfx::setViewTransform(0, nullptr, proj);

	uint64_t state = BGFX_STATE_BLEND_ALPHA |
		BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_TEST_ALWAYS | BGFX_STATE_CULL_CCW;

    for (int n = 0; n < data->CmdListsCount; n++) {
        const ImDrawList* cmdList = data->CmdLists[n];

        bgfx::TransientVertexBuffer tvb;
        bgfx::TransientIndexBuffer tib;

        uint32_t numVertices = (uint32_t)cmdList->VtxBuffer.size();
        uint32_t numIndices = (uint32_t)cmdList->IdxBuffer.size();

		if (!bgfx::checkAvailTransientVertexBuffer(numVertices, VertexPosCoordColor::Decl) ||
			!bgfx::checkAvailTransientIndexBuffer(numIndices))
		{
			break;
		}

        bgfx::allocTransientVertexBuffer(&tvb, numVertices, VertexPosCoordColor::Decl);
        bgfx::allocTransientIndexBuffer(&tib, numIndices);

        // Fill Vertex/Index data
        VertexPosCoordColor* verts = (VertexPosCoordColor*)tvb.data;
        uint16_t* indices = (uint16_t*)tib.data;
        const ImDrawVert* vertexSrc = &cmdList->VtxBuffer[0];

        for (int i = 0; i < cmdList->VtxBuffer.size(); i++) {
            verts->x = vertexSrc->pos.x;
            verts->y = vertexSrc->pos.y;
            verts->tx = vertexSrc->uv.x;
            verts->ty = vertexSrc->uv.y;
            verts->color = vertexSrc->col;

            verts++;
            vertexSrc++;
        }

        memcpy(indices, &cmdList->IdxBuffer[0], cmdList->IdxBuffer.size()*sizeof(ImDrawIdx));

        // Draw Command lists
        uint32_t indexOffset = 0;
        for (int i = 0; i < cmdList->CmdBuffer.size(); i++) {
            const ImDrawCmd& cmd = cmdList->CmdBuffer[i];
            if (cmd.UserCallback) {
                cmd.UserCallback(cmdList, &cmd);
            } else {
                bgfx::setScissor(uint16_t(cmd.ClipRect.x),
                                   uint16_t(cmd.ClipRect.y),
                                   uint16_t(cmd.ClipRect.z - cmd.ClipRect.x),
                                   uint16_t(cmd.ClipRect.w - cmd.ClipRect.y));
                bgfx::TextureHandle* handle = (bgfx::TextureHandle*)cmd.TextureId;
                if (handle)
                    bgfx::setTexture(0, gIm->uniformTexture, *handle);
                bgfx::setState(state);
                bgfx::setIndexBuffer(&tib, indexOffset, cmd.ElemCount);
                bgfx::setVertexBuffer(&tvb, 0, numVertices);
                bgfx::submit(0, gIm->progHandle);
            }

            indexOffset += cmd.ElemCount;
        }
    }
}

int imguiInit(uint16_t viewWidth, uint16_t viewHeight)
{
    if (gIm) {
        assert(false);
        return -1;
    }

    BX_BEGINP("Initializing ImGui Integration");

    bx::AllocatorI* alloc = &gAlloc;
    gIm = BX_NEW(alloc, ImGuiImpl);
    if (!gIm) {
        BX_END_FATAL();
        return -2;
    }
    gIm->alloc = alloc;
    VertexPosCoordColor::init();

    // Create Graphic objects
    bgfx::ShaderHandle fragmentShader = bgfx::createShader(bgfx::makeRef(fs_imgui_h, sizeof(fs_imgui_h)));
	if (!IS_VALID(fragmentShader)) {
        BX_END_FATAL();
		BX_FATAL("Creating fragment-shader failed");
        return -1;
    }

	bgfx::ShaderHandle vertexShader = bgfx::createShader(bgfx::makeRef(vs_imgui_h, sizeof(vs_imgui_h)));
	if (!IS_VALID(vertexShader)) {
        BX_END_FATAL();
		BX_FATAL("Creating vertex-shader failed");
        return -1;
    }

    gIm->progHandle = bgfx::createProgram(vertexShader, fragmentShader, true);
	if (!IS_VALID(gIm->progHandle)) {
        BX_END_FATAL();
		BX_FATAL("Creating GPU Program failed");
        return -1;
    }
    gIm->uniformTexture = bgfx::createUniform("u_texture", bgfx::UniformType::Int1);

    gIm->vertexBuffer = bgfx::createDynamicVertexBuffer(MAX_VERTICES, VertexPosCoordColor::Decl);
    gIm->indexBuffer = bgfx::createDynamicIndexBuffer(MAX_INDICES);
	if (!IS_VALID(gIm->vertexBuffer) || !IS_VALID(gIm->indexBuffer)) {
		BX_END_FATAL();
		BX_FATAL("Creating GPU VertexBuffer failed");
		return -1;
	}

    // Setup ImGui
    ImGuiIO& conf = ImGui::GetIO();
    conf.DisplaySize = ImVec2((float)viewWidth, (float)viewHeight);
    conf.RenderDrawListsFn = imguiDrawLists;
    conf.MemAllocFn = imguiAlloc;
    conf.MemFreeFn = imguiFree;

        conf.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
        conf.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
        conf.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
        conf.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
        conf.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
        conf.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
        conf.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
		conf.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
		conf.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
        conf.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
        conf.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
        conf.KeyMap[ImGuiKey_A] = SDLK_a;
        conf.KeyMap[ImGuiKey_C] = SDLK_c;
        conf.KeyMap[ImGuiKey_V] = SDLK_v;
        conf.KeyMap[ImGuiKey_X] = SDLK_x;
        conf.KeyMap[ImGuiKey_Y] = SDLK_y;
        conf.KeyMap[ImGuiKey_Z] = SDLK_z;

    uint8_t* fontData;
    int fontWidth, fontHeight, bpp;
    conf.Fonts->GetTexDataAsAlpha8(&fontData, &fontWidth, &fontHeight, &bpp);

	gIm->fontTexHandle = bgfx::createTexture2D((uint16_t)fontWidth, (uint16_t)fontHeight, 1, bgfx::TextureFormat::A8,
		BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT,
		bgfx::makeRef(fontData, fontWidth*fontHeight*bpp));
	if (!IS_VALID(gIm->fontTexHandle)) {
        BX_END_FATAL();
		BX_FATAL("ImGui: Could not create font texture");
        return -1;
    }
    conf.Fonts->TexID = (void*)&gIm->fontTexHandle;

    ImGui::NewFrame();

    BX_END_OK();
    return 0;
}

void imguiShutdown()
{
    if (!gIm)
        return;

    bx::AllocatorI* alloc = gIm->alloc;

    ImGui::Shutdown();

    if (IS_VALID(gIm->uniformTexture))
        bgfx::destroyUniform(gIm->uniformTexture);
    if (IS_VALID(gIm->indexBuffer))
        bgfx::destroyDynamicIndexBuffer(gIm->indexBuffer);
    if (IS_VALID(gIm->vertexBuffer))
        bgfx::destroyDynamicVertexBuffer(gIm->vertexBuffer);
    if (IS_VALID(gIm->fontTexHandle))
        bgfx::destroyTexture(gIm->fontTexHandle);
    if (IS_VALID(gIm->progHandle))
        bgfx::destroyProgram(gIm->progHandle);

    BX_DELETE(alloc, gIm);
    gIm = nullptr;
}

void imguiRender()
{
    ImGui::Render();
}

void imguiNewFrame()
{
    ImGui::NewFrame();
    ImGui::ShowTestWindow();
    //ImGui::ShowStyleEditor();
}
