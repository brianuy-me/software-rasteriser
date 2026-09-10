#include <stdio.h>
#include <stdbool.h>

// SDL3 includes
#include <SDL3/SDL.h>

// initialising global variables
bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

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
        800,                        // width
        600,                        // height
        SDL_WINDOW_RESIZABLE
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
    
    
    return 0;
}
