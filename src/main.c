#include <stdio.h>      // input/output
#include <stdint.h>     // _t types
#include <stdbool.h>    // boolen
#include <stdlib.h>     // memory allocation (malloc)
#include <SDL3/SDL.h>   // SDL3 API

#include <display.h>
#include <vector.h>
#include <mesh.h>
#include <triangle.h>

triangle_t triangles_to_render[N_MESH_FACES];

vec3_t camera_position = { .x = 0, .y = 0, .z = -5 };
vec3_t cube_rotation = { .x = 0, .y = 0, .z = 0 };

float fov_factor = 640; // scaling between points

bool is_running = false;
uint64_t previous_frame_time = 0;

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

// takes in a vec3's point convert it to a vec2 
vec2_t project(vec3_t point) {
	vec2_t projected_point = {
		.x = (fov_factor * point.x) / point.z,
		.y = (fov_factor * point.y) / point.z
	};
	return projected_point;
}

void update(void) {
	// 30 fps
	uint64_t elapsed = SDL_GetTicks() - previous_frame_time;
	if(elapsed < FRAME_TARGET_TIME) {
		SDL_Delay((uint32_t)(FRAME_TARGET_TIME - elapsed));
	}
	previous_frame_time = SDL_GetTicks();
	
	cube_rotation.x += 0.01f;
	cube_rotation.y += 0.01f;
	cube_rotation.z += 0.01f;
	
	for(int i = 0; i < N_MESH_FACES; i++) {
		face_t mesh_face = mesh_faces[i];
		
		vec3_t face_vertices[3];
		face_vertices[0] = mesh_vertices[mesh_face.a - 1];
		face_vertices[1] = mesh_vertices[mesh_face.b - 1];
		face_vertices[2] = mesh_vertices[mesh_face.c - 1];
		
		triangle_t projected_triangle;
		
		// loop all three vertices of this current face and apply  transformations
		for(int j = 0; j < 3; j++) {
			vec3_t transformed_vertex = face_vertices[j];
			
			transformed_vertex = vec3_rotate_x(transformed_vertex, cube_rotation.x);
			transformed_vertex = vec3_rotate_y(transformed_vertex, cube_rotation.y);
			transformed_vertex = vec3_rotate_z(transformed_vertex, cube_rotation.z);
			
			// translate the vertex away from the camera
			transformed_vertex.z -= camera_position.z;
			
			// project the current vertex
			vec2_t projected_point = project(transformed_vertex);
			
			// scale and translate the projected points to the middle of the screen
			projected_point.x += (window_width / 2);
			projected_point.y += (window_height / 2);
			
			projected_triangle.points[j] = projected_point;
		}
		// save the projected triangle in the array of triangle to render
		triangles_to_render[i] = projected_triangle;
	}
}

void render(void) {
    draw_grid(0x333333FF);
	
	
	// loop all projected triangles and render them
	for(int i = 0; i < N_MESH_FACES; i++) {
		triangle_t triangle = triangles_to_render[i];
		draw_rect((int)triangle.points[0].x, (int)triangle.points[0].y, 3, 3, 0xFFFF00FF);
		draw_rect((int)triangle.points[1].x, (int)triangle.points[1].y, 3, 3, 0xFFFF00FF);
		draw_rect((int)triangle.points[2].x, (int)triangle.points[2].y, 3, 3, 0xFFFF00FF);
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
	
    
    while(is_running) {
        process_input();
        update();
        render();
    }
    
    // clean-up
    destroy_window();
    return 0;
}
