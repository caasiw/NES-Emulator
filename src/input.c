#include <stdint.h>
#include "./../lib/SDL/include/SDL2/SDL.h"
#include "input.h"

SDL_Event event;

uint8_t pollController1() {
    uint8_t temp = 0x00;
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_X])
        temp = temp | (1 << 7);
    if (keyboardState[SDL_SCANCODE_Z])
        temp = temp | (1 << 6);
    if (keyboardState[SDL_SCANCODE_Q])
        temp = temp | (1 << 5);
    if (keyboardState[SDL_SCANCODE_W])
        temp = temp | (1 << 4);
    if (keyboardState[SDL_SCANCODE_UP])
        temp = temp | (1 << 3);
    if (keyboardState[SDL_SCANCODE_DOWN])
        temp = temp | (1 << 2);
    if (keyboardState[SDL_SCANCODE_LEFT])
        temp = temp | (1 << 1);
    if (keyboardState[SDL_SCANCODE_RIGHT])
        temp = temp | (1 << 0);

    return temp;
}

uint8_t pollController2() {
    uint8_t temp = 0x00;
    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_T])
        temp = temp | (1 << 7);
    if (keyboardState[SDL_SCANCODE_R])
        temp = temp | (1 << 6);
    if (keyboardState[SDL_SCANCODE_ESCAPE])
        temp = temp | (1 << 5);
    if (keyboardState[SDL_SCANCODE_P])
        temp = temp | (1 << 4);
    if (keyboardState[SDL_SCANCODE_I])
        temp = temp | (1 << 3);
    if (keyboardState[SDL_SCANCODE_K])
        temp = temp | (1 << 2);
    if (keyboardState[SDL_SCANCODE_J])
        temp = temp | (1 << 1);
    if (keyboardState[SDL_SCANCODE_L])
        temp = temp | (1 << 0);

    return temp;
}

int pollEvents() {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return 0;
        }
    }
    return 1;
}