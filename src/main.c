#include <stdio.h>      // input/output
#include <stdint.h>     // _t types
#include <stdbool.h>    // boolen
#include <stdlib.h>     // memory allocation (malloc)
#include <SDL3/SDL.h>   // SDL3 API

#include <display.h>
#include <vector.h>

//const int N_POINTS = 9 * 9 * 9;
#define N_POINTS (9*9*9)
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };

float fov_factor = 640;

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
	
	
	// start loading array of vectors (9x9x9)
	int point_count = 0;
	for(float x = -1; x <= 1; x += 0.25) {
		for(float y = -1; y <= 1; y += 0.25) {
			for(float z = -1; z <= 1; z += 0.25) {
				vec3_t new_point = {
					.x = x, 
					.y = y,
					.z = z
				};
				cube_points[point_count++] = new_point;
			}
		}
	}	
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

vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};
	
	return projected_point;
}

void update(void) {
    for(int i = 0; i < N_POINTS; i++) {
		vec3_t point = cube_points[i];
		
		// move the points away from the camera
		point.z -= camera_position.z;
		
		// project the current point
		vec2_t projected_point = project(point);
		
		// save the projected 2d vector in the array of projected points
		projected_points[i] = projected_point;
	}
}

void render(void) {
    draw_grid(0x333333FF);
	
	// loop all projected points and render them
	for(int i = 0; i < N_POINTS; i++) {
		vec2_t projected_point = projected_points[i];
		draw_rectangle(
			(int)projected_point.x + window_width/2, 
			(int)projected_point.y + window_height/2, 
			4,
			4,
			0xFFFF00FF
		);
	}

    
	
	render_color_buffer();                                      // update & render texture
    clear_color_buffer(0x525365FF);                             // set each pixel to yellow (RGBA)
    
    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;
    
    is_running = initialize_window();
    
    setup();
	
    
    while (is_running) {
        process_input();
        update();
        render();
    }
    
    // clean-up
    destroy_window();
    return 0;
}
