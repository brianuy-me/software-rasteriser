#include <stdio.h>      // input/output
#include <stdint.h>     // _t types
#include <stdbool.h>    // boolen
#include <stdlib.h>     // memory allocation (malloc)
#include <SDL3/SDL.h>   // SDL3 API


// global variables
bool is_running = false;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *fb_texture = NULL;
uint32_t *frame_buffer = NULL;

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
      fprintf(stderr, "Error, creating SDL window \n", SDL_GetError());
      return false;
    }

    // setting windows position, returns bool in SDL3
    if (!SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED)) {
        fprintf(stderr, "Error, setting position SDL window \n", SDL_GetError());
        return false; 
    }  
    SDL_ShowWindow(window);
    
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    
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
    frame_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    if (!frame_buffer) { printf("mallac failed to allocate"); }     // check it is not NULL
    
    fb_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
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
// render frame buffer
////////////////////////////////////////////////////////////////////////////////////////////////
// update & render texture
void render_frame_buffer(void) {
    SDL_UpdateTexture(
        fb_texture,
        NULL,
        frame_buffer,
        (int) window_width * sizeof(uint32_t)
    );
    
    if (!SDL_RenderTexture(renderer, fb_texture, NULL, NULL)) {
        fprintf(stderr, "Error, renderer could not be created \n");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// clear frame buffer
////////////////////////////////////////////////////////////////////////////////////////////////
// change color for every pixel
void clear_frame_buffer(uint32_t color) {
    for (int y = 0; y < window_height; y++) {
        for ( int x = 0; x < window_width; x++) {
            frame_buffer[(window_width * y) + x] = color;
        }
    }    
}

////////////////////////////////////////////////////////////////////////////////////////////////
// draw rectangle
////////////////////////////////////////////////////////////////////////////////////////////////
void draw_rectangle(int x, int y, int width, int height, uint32_t color) {
    for (int i = y; i < (y + height); i++) {
        for (int j = x; j < (x + width); j++) {
            frame_buffer[(window_width * i) + j] = color;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
// draw grid
////////////////////////////////////////////////////////////////////////////////////////////////
void draw_grid(uint32_t color) {    
    for (int y = 0; y < window_height; y+=10) {
        for (int x = 0; x < window_width; x+=10) {
            frame_buffer[(window_width * y) + x] = color;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
// render
////////////////////////////////////////////////////////////////////////////////////////////////
void render(void) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);          // set screen colour (RGBA)
    SDL_RenderClear(renderer);                                  // clear whole window
    
    draw_grid(0x333333FF);
    draw_rectangle(40, 40, 200, 140, 0xFF0000FF);
    render_frame_buffer();                                      // update & render texture
    clear_frame_buffer(0x525365FF);                             // set each pixel to yellow (RGBA)
    
    SDL_RenderPresent(renderer);
}

////////////////////////////////////////////////////////////////////////////////////////////////
// memory cleanup
////////////////////////////////////////////////////////////////////////////////////////////////
// destroy memory in reverse order of creation
void memory_cleanup(void) {
    
    free(frame_buffer);
    frame_buffer = NULL;
    SDL_DestroyTexture(fb_texture);   // needs renderer to be alive
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
