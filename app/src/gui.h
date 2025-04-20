#ifndef GUI_H
#define GUI_H

#include "rendering/renderer.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS

#include "external/cimgui/cimgui.h"

typedef struct gui {
    ImTextureID compute_tex[2];

    struct {
        float lx;
        float ly;
        float dist;
        vec3s offset;
    } camera;
} gui;

void gui_start(gui* gui, renderer* r);
void gui_draw(gui* gui, renderer* r, const ig_window* window, const ig_keyboard* keyboard, const ig_mouse* mouse, bool* close_requested);
void gui_end(gui* gui);

#endif