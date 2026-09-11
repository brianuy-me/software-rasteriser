#include <stdio.h>      // input/output
#include <stdint.h>     // _t types
#include <stdbool.h>    // boolen
#include <stdlib.h>     // memory allocation (malloc)
#include <SDL3/SDL.h>   // SDL3 API


// global variables
bool is_running = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL;

int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
    // initialise SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Error, problem initializing SDL. \n", SDL_GetError());
        return false;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // window
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // creating SDL window, assigning window value
    window = SDL_CreateWindow(
        "rasteriser",               // window name
        window_width,               // width
        window_height,              // height
        SDL_WINDOW_RESIZABLE        // window type
    );
    // check if window is working
    if (!window) {
        fprintf(stderr, "Error, creating SDL window \n", SDL_GetError());
        return false;
    }
    // setting windows position, returns bool in SDL3
    if (!SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
        fprintf(stderr, "Error, setting position SDL window \n", SDL_GetError());
        return false; 
    }  
    SDL_ShowWindow(window);
    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // renderer
    ////////////////////////////////////////////////////////////////////////////////////////////////
    
    // creating SDL renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "Error, renderer could not be created \n", SDL_GetError());
        return false;
    }
    
    return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////
// setup
////////////////////////////////////////////////////////////////////////////////////////////////
void setup(void) {
    // memory allocation for color buffer in bytes
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    if (!color_buffer) { printf("mallac failed to allocate"); }     // check it is not NULL
    
    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
    
}


////////////////////////////////////////////////////////////////////////////////////////////////
// process input
////////////////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////////////
// update
////////////////////////////////////////////////////////////////////////////////////////////////
void update(void) {
    
}

////////////////////////////////////////////////////////////////////////////////////////////////
// render color buffer
////////////////////////////////////////////////////////////////////////////////////////////////
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

////////////////////////////////////////////////////////////////////////////////////////////////
// clear color buffer
////////////////////////////////////////////////////////////////////////////////////////////////
void clear_color_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for ( int x = 0; x < window_width; x++) {
            color_buffer[(window_width * y) + x] = color;
        }
    }    
}

////////////////////////////////////////////////////////////////////////////////////////////////
// render
////////////////////////////////////////////////////////////////////////////////////////////////
void render(void) {
    SDL_SetRenderDrawColor(renderer, 52, 53, 65, 255);          // set screen colour (rgba)
    SDL_RenderClear(renderer);                                  // clear whole window

    render_color_buffer();
    clear_color_buffer(0xFFFFFF00); // ARGB
    
    SDL_RenderPresent(renderer);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// memory cleanup
////////////////////////////////////////////////////////////////////////////////////////////////
// destroy memory in reverse order of creation
void memory_cleanup(void) {
    free(color_buffer);
    color_buffer = NULL;
    SDL_DestroyTexture(color_buffer_texture);   // needs renderer to be alive
    SDL_DestroyRenderer(renderer);      // needs window alive
    SDL_DestroyWindow(window);
    SDL_Quit();
}

////////////////////////////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////////////////////////////
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
    
    
    memory_cleanup();
    return 0;
}
