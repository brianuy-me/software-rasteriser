#include <stdio.h>      // input/output
#include <stdint.h>     // _t types
#include <stdbool.h>    // boolen
#include <stdlib.h>     // memory allocation (malloc)
#include <SDL3/SDL.h>   // SDL3 API


// global variables
bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
    // initialise SDL3
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Error, problem initializing SDL. \n", SDL_GetError());
        return false;
    }
    
    /*================================================================================*/ // window
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
    
    /*================================================================================*/ // renderer
    // creating SDL renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "Error, renderer could not be created \n", SDL_GetError());
        return false;
    }
    
    return true;
}

void setup(void) {
    // memory allocation for color buffer
    color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height);
    
    if (!color_buffer) {
        printf("mallac failed to allocate");
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

void update(void) {
    
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 52, 53, 65, 255);          // set screen colour (rgba)
    SDL_RenderClear(renderer);                                  // clear whole window


    SDL_RenderPresent(renderer);
}


// destroy memory in reverse order of creation
void memory_cleanup(void) {
    free(color_buffer);
    color_buffer = NULL;
    SDL_DestroyRenderer(renderer);      // needs window alive
    SDL_DestroyWindow(window);
    SDL_Quit();
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
    
    
    memory_cleanup();

    return 0;
}
