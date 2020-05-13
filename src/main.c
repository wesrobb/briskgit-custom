#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <SDL.h>

#include "SDL_events.h"
#include "SDL_video.h"
#include "app.h"

static bool g_done;

#define MAX_FRAME_TIMES 100
static float g_frameTimes[MAX_FRAME_TIMES] = {0};
static int g_frameTimesIndex = 0;

#define TARGET_FPS 60
static float g_expectedFrameTimeMs = 1000.0f / TARGET_FPS;

static void OnWindowEvent(SDL_WindowEvent *e)
{
    switch (e->event)
    {
        case SDL_WINDOWEVENT_RESIZED:
            {
                int width = e->data1;
                int height = e->data2;

                printf("Window resized to %d x %d\n", width, height);
                App_OnWindowResized(width, height);
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

            case SDL_WINDOWEVENT:
                OnWindowEvent(&event.window);
                break;

            case SDL_MOUSEMOTION:
                App_OnMouseMoved(&event.motion);
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
        int count = 0;
        for (int i = 0; i < MAX_FRAME_TIMES; i++)
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


static void Run(SDL_Window *window)
{
    while (!g_done)
    {
        uint64_t frameStartTime = SDL_GetPerformanceCounter();
        ProcessEvents();

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
}

int main()
{
    printf("Hello briskgit!\n");

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
        dm.w * 0.8, dm.h * 0.8, 
        SDL_WINDOW_RESIZABLE);

    if (window)
    {
        Run(window);
        SDL_DestroyWindow(window);
    }

    SDL_Quit();

    return 0;
}
