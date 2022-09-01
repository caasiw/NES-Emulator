#include <stdio.h>
#include "./../lib/SDL/include/SDL2/SDL.h"
#include "gui.h"

#define Width 256
#define Height 240
#define Scale 3

SDL_Window* window;
SDL_Surface* surface;

struct COLOUR {
    uint8_t r;
    uint8_t g;
    uint8_t b;
};

struct COLOUR colours[64] = {
    {84,84,84},    {0,30,116},    {8,16,144},    {48,0,136},    {68,0,100},    {92,0,48},     {84,4,0},      {60,24,0},
    {32,42,0},     {8,58,0},      {0,64,0},      {0,60,0},      {0,50,60},     {0,0,0},       {0,0,0},       {0,0,0},
    {152,150,152}, {8,76,196},    {48,50,236},   {92,30,228},   {136,20,176},  {160,20,100},  {152,34,32},   {120,60,0},    
    {84,90,0},     {40,114,0},    {8,124,0},     {0,118,40},    {0,102,120},   {0,0,0},       {0,0,0},       {0,0,0},
    {236,238,236}, {76,154,236},  {120,124,236}, {176,98,236},  {228,84,236},  {236,88,180},  {236,106,100}, {212,136,32},  
    {160,170,0},   {116,196,0},   {76,208,32},   {56,204,108},  {56,180,204},  {60,60,60},    {0,0,0},       {0,0,0},
    {236,238,236}, {168,204,236}, {188,188,236}, {212,178,236}, {236,174,236}, {236,174,212}, {236,180,176}, {228,196,144}, 
    {204,210,120}, {180,222,120}, {168,226,144}, {152,226,180}, {160,214,228}, {160,162,160}, {0,0,0},       {0,0,0}
};

void gui_init() {
    if (SDL_Init(SDL_INIT_VIDEO) > 0) {
        printf("%s\n", SDL_GetError());
    }
    else {
        window = SDL_CreateWindow("Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Width * Scale, Height * Scale, SDL_WINDOW_SHOWN);
        if (window == NULL) {
            printf("%s\n", SDL_GetError());
        }
        else {
            surface = SDL_GetWindowSurface(window);
        }
    }
}

void gui_stop() {
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void gui_update(uint8_t *pixels) {
    struct COLOUR colour;
    for (int y = 0; y < 240; y++) {
        // printf("\n");
        for (int x = 0; x < 256; x++) {
            // printf("%02X ",pixels[(y * 256) + x]);
            colour = colours[pixels[(y * 256) + x] % 64];
            for (int j = 0; j < Scale; j++) {            
                for (int i = 0; i < Scale; i++) {
                    ((uint8_t*)surface->pixels)[ ( (((y * Scale) + j) * surface->pitch) + (((x * Scale) + i) * surface->format->BytesPerPixel) + 0) ] = colour.b;
                    ((uint8_t*)surface->pixels)[ ( (((y * Scale) + j) * surface->pitch) + (((x * Scale) + i) * surface->format->BytesPerPixel) + 1) ] = colour.g;
                    ((uint8_t*)surface->pixels)[ ( (((y * Scale) + j) * surface->pitch) + (((x * Scale) + i) * surface->format->BytesPerPixel) + 2) ] = colour.r;
                }
            }


        }
    }
    SDL_UpdateWindowSurface(window);
}