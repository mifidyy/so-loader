/*

        This is a .so loader, that can actively load/unload/reload .so files. since relaunching via LD_PRELOAD
        every time would be a PAIN in the ass.
        
        The cheat itself has 2 functions that also interact with this init() and shutdown() that initialize and 
        clean up the hooks. This Loader calls them after loading / before unloading.

*/      
#include <SDL2/SDL.h>
#include <dlfcn.h>
//this is straight out of the claude kitchen :) for logging
#define LOG(...) do { fprintf(stderr, "[loader] " __VA_ARGS__); fprintf(stderr, "\n"); } while(0)


static void* handle = nullptr;
static bool loaded{false};
//is this smart??? do i know what static does? NO!
static const char* path{"~/inject/tf2/a.so"};

//Function for loading the .so
// "~/inject/tf2/a.so"

void load() {
    handle = dlopen(path, RTLD_NOW | RTLD_LOCAL); //inject the dll
    if (!handle) { fprintf(stderr, "%s\n", dlerror()); return; } //if theres a error log it
    auto init = (void(*)())dlsym(handle, "mod_init"); //Look for the function init
    init(); //call init
    loaded = true;
}

//unloads the cheat, and calls its shutdown / cleanup function

void unload() {
    auto shutdown = (void(*)())dlsym(handle, "mod_shutdown");
    shutdown();          // loaded.so cleans itself up
    dlclose(handle);     // now it's safe to unmap
    handle = nullptr;
    loaded = false;
}

//if you couldnt tell this code is copy pasted, thrown together
//this function looks A: intercepts the SDL_PollEvent function the game (prob) looks for. then basically mitm's it.
// on f5 load; on f6 unload; on f7 reload

extern "C" int SDL_PollEvent(SDL_Event* ev) {
    static auto real = (int(*)(SDL_Event*))dlsym(RTLD_NEXT, "SDL_PollEvent");
    int r = real(ev);
    if (r && ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_F5 && loaded == false) {
        load();
        LOG("F1 detected, ~/inject/tf2/a.so");
    }
    if (r && ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_F6) {
        unload();
        LOG("F2 detected, unloading...");
    }
    if (r && ev->type == SDL_KEYDOWN && ev->key.keysym.sym == SDLK_F7) {
        LOG("F3 detected, reloading...");
        if (loaded == true) {
            unload();
        } 
        load();
    }
    return r;
}