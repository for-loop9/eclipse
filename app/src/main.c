#include "rendering/renderer.h"
#include "gui.h"

#include <external/stb/stb_image.h>

int main(void) {
	ig_window* window = ig_window_create_asp(16.0f / 9.0f, "program", NULL, 0);
	ig_keyboard* keyboard = ig_keyboard_create(window);
	ig_mouse* mouse = ig_mouse_create(window);
	ig_context* context = ig_context_create(window, 1, 0);
	renderer* renderer = renderer_create(context, window);
	bool close_requested = false;

	gui g = {};
	gui_start(&g, renderer);

	printf("window %d %d\n", window->dim.x, window->dim.y);

	while (!ig_window_closed(window) && !close_requested) {
		ig_window_input(window);
		renderer_start_imgui_frame(renderer);

		gui_draw(&g, renderer, window, keyboard, mouse, &close_requested);

		if (ig_context_begin(context, window, 1, 0)) {
			renderer_flush(renderer, &(ig_vec4) { 0, 0, 0, 1.0f });
			ig_context_end(context, window, 1, 0);
		}

		ig_keyboard_update(keyboard);
		ig_mouse_update(mouse);
	}
	
	printf("window exit\n");
	ig_context_finish(context);

	gui_end(&g);

	renderer_destroy(renderer);
	ig_context_destroy(context);
	ig_mouse_destroy(mouse);
	ig_keyboard_destroy(keyboard);
	ig_window_destroy(window);
}
