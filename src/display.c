#include <display.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// global variables definitions
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

static uint32_t *color_buffer = NULL;
static float *z_buffer = NULL;

static SDL_Texture *color_buffer_texture = NULL;
static int window_width = 800;
static int window_height = 600;

static int render_method = 0;
static int cull_method = 0;


int get_window_width(void) {
	return window_width;
}

int get_window_height(void) {
	return window_height;
}

void set_render_method(int method) {
	render_method = method;
}

void set_cull_method(int method) {
	cull_method = method;
}

bool is_cull_backface(void) {
	return cull_method == CULL_BACKFACE;
}

bool should_render_filled_triangles(void) {
	return (
		render_method == RENDER_FILL_TRIANGLE || 
		render_method == RENDER_FILL_TRIANGLE_WIRE
	);
}

bool should_render_textured_triangles(void) {
	return (
		render_method == RENDER_TEXTURED || 
		render_method == RENDER_TEXTURED_WIRE
	);
}

bool should_render_wireframe(void) {
	return (
		render_method == RENDER_WIRE || 
		render_method == RENDER_WIRE_VERTEX || 
		render_method == RENDER_FILL_TRIANGLE_WIRE || 
		render_method == RENDER_TEXTURED_WIRE
	);
}

bool should_render_wire_vertex(void) {
	return (
		render_method == RENDER_WIRE_VERTEX
	);
}

bool initialize_window(void) {
    // initialise SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Error, problem initializing SDL. %s\n", SDL_GetError());
        return false;
    }
    
    // creating SDL window, assigning window value
    window = SDL_CreateWindow(
        "Software Rasteriser",      // window name
        window_width,               // width
        window_height,              // height
        SDL_WINDOW_RESIZABLE        // window type
    );
    // check if window is working
    if (!window) {
      fprintf(stderr, "Error, creating SDL window %s\n", SDL_GetError());
      return false;
    }

    // setting windows position, returns bool in SDL3
    if (!SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
        fprintf(stderr, "Error, setting position SDL window %s\n", SDL_GetError());
        return false; 
    }  
    SDL_ShowWindow(window);
    
	// setting up window's icon
	char icon_path[1024];
	snprintf(icon_path, sizeof icon_path, "%s%s", SDL_GetBasePath(), "cube.bmp");
	SDL_Surface *icon = SDL_LoadBMP(icon_path);
	
	// if icon is NULL return NULL
	if(!icon) {
		fprintf(stderr, "Error, failed to create icon: %s\n", SDL_GetError());
		return false;
	}
	
	// set icon
	if(!SDL_SetWindowIcon(window, icon)) {
		fprintf(stderr, "Error, failed to set window icon: %s\n", SDL_GetError());
		return false;
	}
	
	// free icon after use
	SDL_DestroySurface(icon);
	icon = NULL;

    // creating SDL renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "Error, renderer could not be created %s\n", SDL_GetError());
        return false;
    }
	
	// memory allocation for color buffer in bytes
    color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);
    if (!color_buffer) { printf("mallac failed to allocate"); }     // check it is not NULL
    
	z_buffer = (float *)malloc(sizeof(float) * window_width * window_height);
    if (!z_buffer) { printf("mallac failed to allocate"); }     // check it is not NULL
	
	// creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
	
    return true;
}

void draw_grid(uint32_t color) {    
    for (int y = 0; y < window_height; y+=10) {
        for (int x = 0; x < window_width; x+=10) {
            color_buffer[(window_width * y) + x] = color;
        }
    }
}

void draw_pixel(int x, int y, uint32_t color) {
	if(x < 0 || x >= window_width || y < 0 || y >= window_height) {
		return;
	}
	color_buffer[x + (y * window_width)] = color;
}

void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
	// get the delta value of x and y
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);
	
	// if delta_x is more than or equal to delta_y use delta_x otheisde use delta_y (ternary operator)
	int longest_side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);
	
	// find how much delta_x and delta_y should increment each step
	float x_inc = delta_x / (float)longest_side_length;
	float y_inc = delta_y / (float)longest_side_length;
	
	// get the current location of x and y
	float current_x = (float)x0;
	float current_y = (float)y0;
	
	// increment x and y each loop and draw that pixel
	for(int i = 0; i <= longest_side_length; i++) {
		draw_pixel((int)round(current_x), (int)round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
	for(int i = 0; i < width; i++) {
		for(int j = 0; j < height; j++) {
			int current_x = x + i;
			int current_y = y + j;
			
			draw_pixel(current_x, current_y, color);
			//color_buffer[(window_width * current_y) + current_x] = color;
		}
	}
}

// update & render texture
void render_color_buffer(void) {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int) window_width * sizeof(uint32_t)
    );
    
    if (!SDL_RenderTexture(renderer, color_buffer_texture, NULL, NULL)) {
        fprintf(stderr, "Error, renderer could not be created \n");
    }
	
    SDL_RenderPresent(renderer);
}

// change color for every pixel
void clear_color_buffer(uint32_t color) {
    for (int i = 0; i < window_width * window_height; i++) {
        color_buffer[i] = color;
    }    
}

void clear_z_buffer(void) {
    for (int i = 0; i < window_width * window_height; i++) {
        z_buffer[i] = 1.0f;
    }    
}


float get_zbuffer_at(int x, int y) {
	if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
		return 1.0f;
	}
	return z_buffer[(window_width * y) + x];
}


void update_zbuffer_at(int x, int y, float value) {
	if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
		return;
	}
	z_buffer[(window_width * y) + x] = value;
	
	
}


// destroy memory in reverse order of creation
void destroy_window(void) {	
	free(color_buffer);
    color_buffer = NULL;
	
	free(z_buffer);
	z_buffer = NULL;

	// needs renderer to be alive
    SDL_DestroyTexture(color_buffer_texture);
	color_buffer_texture = NULL;
	
	// needs window alive
    SDL_DestroyRenderer(renderer);
	renderer = NULL;
	
    SDL_DestroyWindow(window);
	renderer = NULL;
	
    SDL_Quit();
}

