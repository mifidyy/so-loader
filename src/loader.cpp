#include <SDL2/SDL.h>
#include <dlfcn.h>

extern "C" int SDL_PollEvent(SDL_Event* ev) {
    static auto real = (int(*)(SDL_Event*))dlsym(RTLD_NEXT, "SDL_PollEvent");
    int r = real(ev);
    if (r && ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_F1) {
        // your code
    }
    return r;
}