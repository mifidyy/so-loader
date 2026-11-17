/*

        This is a .so loader, that can actively load/unload/reload .so files. since relaunching via LD_PRELOAD
        every time would be a PAIN in the ass.
        
        The cheat itself has 2 functions that also interact with this init() and shutdown() that initialize and 
        clean up the hooks. This Loader calls them after loading / before unloading.

*/      
#include <SDL2/SDL.h>
#include <dlfcn.h>
#include "log.hpp"

static void* handle = nullptr;
static bool loaded{false};
//is this smart??? do i know what static does? NO!
static const char* path{"/home/mifidy/inject/tf2/a.so"};

//Function for loading the .so

void load() {
    handle = dlopen(path, RTLD_NOW | RTLD_LOCAL); //inject the dll
    if (!handle) { LOG("%s", dlerror()); return; } //if theres a error log it
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
//update: didnt work with rtdl_next so now we just try to guess libsdl name (yes ik very stable and secure and perfect)

extern "C" int SDL_PollEvent(SDL_Event* ev) {
    // making sure we are getting real function
    static auto real = []() -> int(*)(SDL_Event*) {
        // trying to directly talk to the sdl lib.
        void* handle = dlopen("libSDL2-2.0.so.0", RTLD_NOLOAD | RTLD_LAZY);
        if (!handle) {
            handle = dlopen("libSDL2.so.0", RTLD_LAZY);
        }
        if (handle) {
            auto fn = (int(*)(SDL_Event*))dlsym(handle, "SDL_PollEvent");
            if (fn) return fn;
        }
        // WORST case we use rtld_next which hasnt worked in tests (idk why im even doing this.)
        return (int(*)(SDL_Event*))dlsym(RTLD_NEXT, "SDL_PollEvent");
    }();

    // if we are a process that doesnt even use SDL (so !real) we insta hop OUTTA THERE
    if (!real) {
        return 0; 
    }

    int r = real(ev);
    
    if (r && ev->type == SDL_KEYDOWN) {
        if (ev->key.keysym.sym == SDLK_F5 && !loaded) {
            load();
            LOG("F5 detected, loading...");
        }
        if (ev->key.keysym.sym == SDLK_F6 && loaded) {
            unload();
            LOG("F6 detected, unloading...");
        }
        if (ev->key.keysym.sym == SDLK_F7) {
            LOG("F7 detected, reloading...");
            if (loaded) {
                unload();
            } 
            load();
        }
    }
    return r;
}