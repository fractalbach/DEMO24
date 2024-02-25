#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // only include this one in the source file with main()!
#include <chrono>

#define SCREEN_WIDTH 680
#define SCREEN_HEIGHT 480

typedef struct vec3 {
    double x;
    double y;
    double z;
} vec3;

vec3 framebuffer[SCREEN_HEIGHT][SCREEN_WIDTH];

double time() {
    const auto p1 = std::chrono::system_clock::now();
    return double( std::chrono::duration_cast<std::chrono::milliseconds>(p1.time_since_epoch()).count() ) / 1000.0f;
}

double start_time = time();

/* Fill framebuffer with some stuff */
void draw() {

    const double iTime = time() - start_time;

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            const double u = double(x) / SCREEN_WIDTH;
            const double v = double(y) / SCREEN_HEIGHT;
            
            vec3 color = vec3{
                0.5f + 0.5f * cos( iTime + u ),
                0.5f + 0.5f * cos( iTime + v + 2.0 ),
                0.5f + 0.5f * cos( iTime + u + 4.0 )
            };

            framebuffer[y][x] = color;
        }
    }
}


int main(int argc, char* argv[])
{
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Surface* surface = nullptr;
    SDL_Texture* texture = nullptr;

    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Demo24", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, NULL, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* Main render loop */
    int done = 0;
    while (!done) {
        /* Check for events */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_KEY_DOWN) {
                done = 1;
            }
        }

        /* Do fun drawing functions to the framebuffer, without worrying about SDL stuff */
        draw();

        /* Update Screen */
        SDL_RenderClear(renderer);
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                vec3 color = framebuffer[y][x];
                SDL_SetRenderDrawColor(renderer, color.x * 256, color.y * 256, color.z * 256, 1.0);
                SDL_RenderPoint(renderer, x, y);
            }
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}