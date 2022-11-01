#pragma once

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "imgui/imgui_impl_opengl2.h"
#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <functional>
#include <string>

// This is a test

class Gui
{
    public:
    Gui(int width, int height, std::string title);
    ~Gui();
    bool draw();

    protected:
    SDL_GLContext gl_context;
    SDL_Window *window;
    int previousTime = 0;
    bool isDone = false;
    bool isFirstFrame = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};