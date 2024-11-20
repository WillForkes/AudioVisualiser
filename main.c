//
// Created by William Forkes on 20/11/2024.
//

#include "main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define AUDIO_FREQUENCY 44100
#define AUDIO_CHANNELS 1

Mix_Chunk *audioChunk;
Uint8 *audioPos;
Uint32 audioLen;

// Function to load the audio
int load_audio(const char *filename) {
    // Open the audio device
    if (Mix_OpenAudio(AUDIO_FREQUENCY, MIX_DEFAULT_FORMAT, AUDIO_CHANNELS, 1024) < 0) {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    // Load the audio file
    audioChunk = Mix_LoadWAV(filename);
    if (!audioChunk) {
        printf("Failed to load audio file! SDL_mixer Error: %s\n", Mix_GetError());
        return 1;
    }

    // Set the audio length and position
    audioPos = audioChunk->abuf;
    audioLen = audioChunk->alen;

    return 0;
}

// SDL audio callback function (to read the audio into a buffer)
void audio_callback(void *userdata, Uint8 *stream, int len) {
    // Copy the audio data into the stream
    if (audioLen == 0) {
        return;  // No more audio
    }
    len = (len > audioLen ? audioLen : len);
    SDL_memcpy(stream, audioPos, len);
    audioPos += len;
    audioLen -= len;
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window *win = SDL_CreateWindow("Audio Visualizer", 100, 100, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        SDL_DestroyWindow(win);
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Main loop
    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Clear the screen
        SDL_RenderClear(renderer);

        // code for visualisation


        // Present the screen
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}