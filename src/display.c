#include <display.h>

#include <stdio.h>

// global variables definitions
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;


bool initialize_window(void) {
    // initialise SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Error, problem initializing SDL. %s\n", SDL_GetError());
        return false;
    }
    
    // use SDL to query what is the fullscreen max width and height
    // SDL_DisplayID display = SDL_GetPrimaryDisplay();
    // const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(display);
    
    // window_width = mode->w;
    // window_height = mode->h;

    // creating SDL window, assigning window value
    window = SDL_CreateWindow(
        "rasteriser",               // window name
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
    
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    
    // creating SDL renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "Error, renderer could not be created %s\n", SDL_GetError());
        return false;
    }
    
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
	if(x < window_width && y < window_height) {
		color_buffer[x + (y * window_width)] = color;
	}
}



void draw_rectangle(int x, int y, int width, int height, uint32_t color) {
    for (int i = y; i < (y + height); i++) {
        for (int j = x; j < (x + width); j++) {
            color_buffer[(window_width * i) + j] = color;
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
}

// change color for every pixel
void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for ( int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }    
}

// destroy memory in reverse order of creation
void destroy_window(void) {
	
    free(color_buffer);
    color_buffer = NULL;
	
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

