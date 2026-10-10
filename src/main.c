#include <display.h>
#include <vector.h>

#include <stdio.h>      // input/output
#include <stdint.h>     // _t types
#include <stdbool.h>    // boolen
#include <stdlib.h>     // memory allocation (malloc)
#include <SDL3/SDL.h>   // SDL3 API

bool is_running = false;

void setup(void) {
    // memory allocation for color buffer in bytes
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    if (!color_buffer) { printf("mallac failed to allocate"); }     // check it is not NULL
    
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);
    
    switch (event.type) {
        case SDL_EVENT_QUIT:
            is_running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (event.key.key == SDLK_ESCAPE)
                is_running = false;
            break;
    }
}

void update(void) {
    
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);          // set screen colour (RGBA)
    SDL_RenderClear(renderer);                                  // clear whole window
    
    draw_grid(0x333333FF);
	
	
	draw_pixel(30, 30, 0xFFFF00FF);
    draw_rectangle(40, 40, 200, 140, 0xFF0000FF);
    
	
	render_color_buffer();                                      // update & render texture
    clear_color_buffer(0x525365FF);                             // set each pixel to yellow (RGBA)
    
    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    is_running = initialize_window();
    
    setup();
	
	vec3_t my_vector = { 2.0, 3.0, -4.0 };
    
    while (is_running) {
        process_input();
        update();
        render();
    }
    
    // clean-up
    destroy_window();
    return 0;
}
