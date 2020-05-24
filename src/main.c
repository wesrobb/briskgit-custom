#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>

#include "SDL_assert.h"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_video.h"
#include "app.h"
#include "render.h"

static bool g_done;

#define MAX_FRAME_TIMES 100
static float g_frameTimes[MAX_FRAME_TIMES] = {0};
static int32_t g_frameTimesIndex = 0;

#define TARGET_FPS 60
static float g_expectedFrameTimeMs = 1000.0f / TARGET_FPS;

typedef struct RenderContext
{
    int32_t windowHeight, windowWidth;
    int32_t renderHeight, renderWidth;
    float   scaleFactorX, scaleFactorY;

    SDL_Renderer *renderer;
    SDL_Texture *texture;

    SDL_Window *window; // The window is not owned by the render context.
} RenderContext;

static void CreateRenderContext(RenderContext *renderContext, SDL_Window *window)
{
    SDL_assert(renderContext);
    SDL_assert(window);

    puts("Creating render context");
    renderContext->window = window;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer)
    {
        SDL_GetRendererOutputSize(renderer, &renderContext->renderWidth, &renderContext->renderHeight);
        printf("Renderer output size is %d x %d\n", renderContext->renderWidth, renderContext->renderHeight);

        SDL_GetWindowSize(window, &renderContext->windowWidth, &renderContext->windowHeight);
        printf("Window size is %d x %d\n", renderContext->windowWidth, renderContext->windowHeight);

        renderContext->scaleFactorX = (float)renderContext->renderWidth / (float)renderContext->windowWidth;
        renderContext->scaleFactorY = (float)renderContext->renderHeight / (float)renderContext->windowHeight;
        printf("Width scaling %.2f\n", renderContext->scaleFactorX);
        printf("Height scaling %.2f\n", renderContext->scaleFactorY);

        SDL_Texture *texture = SDL_CreateTexture(renderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                renderContext->renderWidth, renderContext->renderHeight);
        if (texture)
        {
            renderContext->texture = texture;
        }
        else
        {
            printf("Failed to create SDL texture %s\n", SDL_GetError());
        }

        renderContext->renderer = renderer;
    }
    else
    {
        printf("Failed to create SDL renderer %s\n", SDL_GetError());
    }
}

static void DestroyRenderContext(RenderContext *renderContext)
{
    SDL_assert(renderContext);

    puts("Destroying render context");

    if (renderContext->texture)
    {
        SDL_DestroyTexture(renderContext->texture);
        renderContext->texture = NULL;
    }

    if (renderContext->renderer)
    {
        SDL_DestroyRenderer(renderContext->renderer);
        renderContext->renderer = NULL;
    }

    renderContext->renderWidth = 0;
    renderContext->renderHeight = 0;
    renderContext->scaleFactorX = 0.0f;
    renderContext->scaleFactorY = 0.0f;
}

// This enables support for redrawing the window resizes.
static int WindowEventWatcher(void* data, SDL_Event* event) {
    RenderContext *renderContext = (RenderContext*)data;

    if (event->type == SDL_WINDOWEVENT)
    {
        if (event->window.event == SDL_WINDOWEVENT_RESIZED ||
            event->window.event == SDL_WINDOWEVENT_MOVED) 
        {
            DestroyRenderContext(renderContext);
            CreateRenderContext(renderContext, renderContext->window);
            Render_Update(
                    renderContext->renderWidth, renderContext->renderHeight,
                    renderContext->scaleFactorX, renderContext->scaleFactorY);
            App_Draw();

            FrameBuffer *fb = Render_GetFrameBuffer();
            int32_t pitchBytes = fb->width * (int32_t)sizeof(Pixel);
            SDL_UpdateTexture(renderContext->texture, NULL, fb->pixels, pitchBytes);
            SDL_RenderCopy(renderContext->renderer, renderContext->texture, 0, 0);
            SDL_RenderPresent(renderContext->renderer);
        }
    }
    return 0;
}

static void OnKeyReleased(SDL_Keysym *keysym)
{
    if (keysym->sym == SDLK_ESCAPE)
    {
        g_done = true;
    }
}

static void ProcessEvents()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                g_done = true;
                break;

            case SDL_KEYUP:
                OnKeyReleased(&event.key.keysym);
                break;

            case SDL_MOUSEMOTION:
                App_OnMouseMoved(&event.motion);
                break;

            case SDL_MOUSEBUTTONDOWN:
                App_OnMousePressed(&event.button);
                break;

            case SDL_MOUSEBUTTONUP:
                App_OnMouseReleased(&event.button);
                break;
        }
    }
}

static float ElapsedMs(uint64_t start, uint64_t end)
{
    SDL_assert(end >= start);
    return (float)((end - start)*1000) / SDL_GetPerformanceFrequency();
}

static void PushFrameTime(float frameTimeMs)
{
    g_frameTimes[g_frameTimesIndex] = frameTimeMs;
    g_frameTimesIndex = (g_frameTimesIndex + 1) % MAX_FRAME_TIMES;
}

static void LogFrameTime(float frameTimeMs)
{
    PushFrameTime(frameTimeMs);

    static uint64_t lastLoggedAt = 0;
    uint64_t currentTime = SDL_GetPerformanceCounter();
    float elapsedMs = ElapsedMs(lastLoggedAt, currentTime);
    if (elapsedMs > 2000.0f)
    {
        float sum = 0.0f;
        int32_t count = 0;
        for (int32_t i = 0; i < MAX_FRAME_TIMES; i++)
        {
            float frameTimeMs = g_frameTimes[i];
            if (frameTimeMs == 0.0f) {
                break;
            }
            sum += frameTimeMs;
            count++;
        }

        float averageFrameTimeMs = sum / (float)count;
        printf("Frame time: %.1f\n", averageFrameTimeMs);
        lastLoggedAt = currentTime;
    }
}

static void Run(SDL_Window *window, RenderContext *renderContext)
{
    if (Render_Init(renderContext->renderWidth, renderContext->renderHeight, renderContext->scaleFactorX, renderContext->scaleFactorY))
    {
        App_Init();

        while (!g_done)
        {
            uint64_t frameStartTime = SDL_GetPerformanceCounter();
            ProcessEvents();
            App_Draw();

            FrameBuffer *fb = Render_GetFrameBuffer();
            int32_t pitchBytes = fb->width * (int32_t)sizeof(Pixel);
            SDL_UpdateTexture(renderContext->texture, NULL, fb->pixels, pitchBytes);
            SDL_RenderCopy(renderContext->renderer, renderContext->texture, 0, 0);
            SDL_RenderPresent(renderContext->renderer);

            uint64_t currentTime = SDL_GetPerformanceCounter();
            float frameTimeMs = ElapsedMs(frameStartTime, currentTime);
            LogFrameTime(frameTimeMs);

            if (g_expectedFrameTimeMs > frameTimeMs)
            {
                float sleepMs = g_expectedFrameTimeMs - frameTimeMs;
                SDL_Delay((uint32_t)sleepMs);
            }

            unsigned windowFlags = SDL_GetWindowFlags(window);
            bool windowHasFocus = windowFlags & SDL_WINDOW_INPUT_FOCUS;
            if (!windowHasFocus)
            {
                SDL_Delay(100);
            }
        }
        App_Destroy();
        Render_Destroy();
    }
}

SDL_AssertState AssertionHandler(const SDL_AssertData* data,
                                 void*                 userdata)
{
    (void)userdata;

    printf("SDL assertion triggered\n");
    printf("\tfilename: %s\n", data->filename);
    printf("\tlinenum: %d\n", data->linenum);
    printf("\tfunction: %s\n", data->function);
    printf("\tcondition: %s\n", data->condition);
    printf("SDL assertion triggered\n");

    return SDL_ASSERTION_ABORT;
}

int main()
{
    printf("Hello briskgit!\n");

    SDL_SetAssertionHandler(AssertionHandler, 0);

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    SDL_EnableScreenSaver();

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    printf("Display width:   %d\n", dm.w);
    printf("Display height:  %d\n", dm.h);
    printf("Display refresh: %dhz\n", dm.refresh_rate);

    SDL_Window *window = SDL_CreateWindow(
        "briskgit",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        (int32_t)(dm.w * 0.8), (int32_t)(dm.h * 0.8),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window)
    {
        RenderContext renderContext;
        CreateRenderContext(&renderContext, window);

        // Watch for window events outside the main loop
        // to allow continuous drawing as the window is
        // resized.
        SDL_AddEventWatch(WindowEventWatcher, &renderContext);

        Run(window, &renderContext);
        DestroyRenderContext(&renderContext);
        SDL_DestroyWindow(window);
    }

    SDL_Quit();

    return 0;
}
