#pragma once
//this is straight out of the claude kitchen :) for logging
#define LOG(...) do { fprintf(stderr, "[loader] " __VA_ARGS__); fprintf(stderr, "\n"); } while(0)
