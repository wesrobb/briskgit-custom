#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>

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

static int32_t GetDisplayDpiFromWindow(SDL_Window *window, float *dpiX, float *dpiY)
{
    int32_t displayIndex = SDL_GetWindowDisplayIndex(window);

    float unused;
    int32_t error = SDL_GetDisplayDPI(displayIndex, &unused, dpiX, dpiY);
    if (error)
    {
        printf("Error on window move %s\n", SDL_GetError());
        return error;
    }

    return 0;
}

// Returns the DPI of display on which the application window resides.
static int32_t GetDisplayDpiFromWindowId(uint32_t windowId, float *dpiX, float *dpiY)
{
    SDL_assert(dpiX);
    SDL_assert(dpiY);

    SDL_Window *window = SDL_GetWindowFromID(windowId);
    if (window)
    {
        return GetDisplayDpiFromWindow(window, dpiX, dpiY);
    }

    return 1;
}

static void OnWindowEvent(SDL_WindowEvent *e, SDL_Renderer *renderer, SDL_Texture *texture)
{
    switch (e->event)
    {
        case SDL_WINDOWEVENT_RESIZED:
            {
                int32_t windowWidth = e->data1;
                int32_t windowHeight = e->data2;
                printf("Window resized to %d x %d\n", windowWidth, windowHeight);

                int32_t width, height;
                SDL_GetRendererOutputSize(renderer, &width, &height);
                printf("Renderer output size is %d x %d\n", width, height);

                float dpiX, dpiY;
                GetDisplayDpiFromWindowId(e->windowID, &dpiX, &dpiY);
                SDL_DestroyTexture(texture);
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
                Render_Update(width, height, dpiX, dpiY, 2.0f, 2.0f);
            }
            break;
        case SDL_WINDOWEVENT_MOVED:
            {
                int32_t width, height;
                SDL_GetRendererOutputSize(renderer, &width, &height);
                printf("Renderer output size is %d x %d\n", width, height);

                float dpiX, dpiY;
                GetDisplayDpiFromWindowId(e->windowID, &dpiX, &dpiY);
                SDL_DestroyTexture(texture);
                texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
                Render_Update(width, height, dpiX, dpiY, 2.0f, 2.0f);
                printf("Window moved\n");
                printf("dpiX is %f\n", dpiX);
                printf("dpiY is %f\n", dpiY);
            }
            break;
    }
}

static void OnKeyReleased(SDL_Keysym *keysym)
{
    if (keysym->sym == SDLK_ESCAPE)
    {
        g_done = true;
    }
}

static void ProcessEvents(SDL_Renderer *renderer, SDL_Texture *texture)
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

            case SDL_WINDOWEVENT:
                OnWindowEvent(&event.window, renderer, texture);
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


static void Run(SDL_Window *window, SDL_Renderer *renderer)
{
    int32_t renderWidth, renderHeight;
    SDL_GetRendererOutputSize(renderer, &renderWidth, &renderHeight);
    printf("Renderer output size is %d x %d\n", renderWidth, renderHeight);

    int32_t windowWidth, windowHeight;
    SDL_GetWindowSize(window, &windowWidth, &windowHeight);
    printf("Window size is %d x %d\n", windowWidth, windowHeight);

    float scaleFactorX = (float)renderWidth / (float)windowWidth;
    float scaleFactorY = (float)renderHeight / (float)windowHeight;
    printf("Width scaling %.2f\n", scaleFactorX);
    printf("Height scaling %.2f\n", scaleFactorY);

    float dpiX, dpiY;
    GetDisplayDpiFromWindow(window, &dpiX, &dpiY);

    if (Render_Init(renderWidth, renderHeight, dpiX, dpiY, scaleFactorX, scaleFactorY))
    {
        App_Init();

        SDL_Texture *texture = SDL_CreateTexture(renderer,
                SDL_PIXELFORMAT_ARGB8888,
                SDL_TEXTUREACCESS_STREAMING,
                renderWidth, renderHeight);
        if (texture)
        {
            while (!g_done)
            {
                uint64_t frameStartTime = SDL_GetPerformanceCounter();
                ProcessEvents(renderer, texture);

                App_Draw();

                FrameBuffer *fb = Render_GetFrameBuffer();
                int32_t pitchBytes = fb->width * (int32_t)sizeof(Pixel);
                SDL_UpdateTexture(texture, NULL, fb->pixels, pitchBytes);
                SDL_RenderCopy(renderer, texture, 0, 0);
                SDL_RenderPresent(renderer);

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
            SDL_DestroyTexture(texture);
        }
        App_Destroy();
    }
}

int main()
{
    printf("Hello briskgit!\n");

    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");

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
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer)
        {
            Run(window, renderer);
            SDL_DestroyRenderer(renderer);
        }
        SDL_DestroyWindow(window);
    }

    SDL_Quit();

    return 0;
}
