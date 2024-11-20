#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <fftw3.h>
#include <stdio.h>
#include <stdlib.h>

// Define window dimensions
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define AUDIO_FREQUENCY 44100  // Standard audio sampling frequency
#define AUDIO_CHANNELS 1       // Mono audio channel for simplicity

// Global variables for SDL_mixer
Mix_Chunk *audioChunk;
Uint8 *audioPos;   // Pointer to the audio position
Uint32 audioLen;   // Total length of the audio

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

int main(int argc, char *argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize SDL_mixer
    if (load_audio("your_audio_file.wav") != 0) {
        return 1;
    }

    // Create a window
    SDL_Window *window = SDL_CreateWindow("Audio Visualizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set up FFTW (1D FFT)
    int N = 1024;  // Number of FFT points (you can adjust based on the desired frequency resolution)
    fftw_complex *in = fftw_malloc(sizeof(fftw_complex) * N);
    fftw_complex *out = fftw_malloc(sizeof(fftw_complex) * N);
    fftw_plan p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    // Start playing the audio
    Mix_PlayChannel(-1, audioChunk, 0);

    // Main loop
    int running = 1;
    SDL_Event e;
    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Get the audio data into the FFT input buffer
        // (For simplicity, we simulate this with dummy data here)
        for (int i = 0; i < N; i++) {
            in[i][0] = (float)(audioPos[i % audioLen]); // Simulating audio signal as real part
            in[i][1] = 0.0f;  // Imaginary part is zero
        }

        // Perform FFT
        fftw_execute(p);

        // Render frequency data (just an example visualization)
        SDL_RenderClear(renderer);
        for (int i = 0; i < N / 2; i++) {  // Only half the data is needed for visualization
            int height = (int)(out[i][0] * 100);  // Example, scale frequency data to fit the window
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);  // Blue bars
            SDL_RenderDrawLine(renderer, i * (WINDOW_WIDTH / N), WINDOW_HEIGHT, i * (WINDOW_WIDTH / N), WINDOW_HEIGHT - height);
        }

        // Present rendered content
        SDL_RenderPresent(renderer);
        SDL_Delay(100);  // Update every 100ms
    }

    // Clean up FFTW
    fftw_destroy_plan(p);
    fftw_free(in);
    fftw_free(out);

    // Clean up SDL and SDL_mixer
    Mix_FreeChunk(audioChunk);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_CloseAudio();
    SDL_Quit();

    return 0;
}