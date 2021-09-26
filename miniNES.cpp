#include <filesystem>
#include <SDL2/SDL.h>
#include "machine/Machine.h"

const int WIDTH = 256;
const int HEIGHT = 240;
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;


int main(int argc, char* args[])
{
	std::string filename(args[1]);
	std::filesystem::path path(filename);
	std::string title = "miniNES - " + path.stem().string();

	SDL_Event event;
	SDL_AudioSpec want, have;
	SDL_memset(&want, 0, sizeof(want));
	want.freq = 48000;
	want.format = AUDIO_S16;
	want.channels = 1;
	want.silence = 0;

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	auto keystate = SDL_GetKeyboardState(NULL);
	auto window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	auto renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	auto texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
	auto audio = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
	SDL_PauseAudioDevice(audio, 0);

	Machine nes;
	nes.load(filename);
	nes.reset();

	for (bool quit = false; !quit;)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				quit = true;
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)
				quit = true;
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_r)
				nes.reset();
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_1)
				nes.change(0);
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_2)
				nes.change(1);
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_3)
				nes.change(2);
			else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_4)
				nes.change(3);
		}

		uint8_t input = 0;
		if (keystate[SDL_SCANCODE_LCTRL])
			input |= 0x01;
		if (keystate[SDL_SCANCODE_LALT])
			input |= 0x02;
		if (keystate[SDL_SCANCODE_RSHIFT])
			input |= 0x04;
		if (keystate[SDL_SCANCODE_RETURN])
			input |= 0x08;
		if (keystate[SDL_SCANCODE_UP])
			input |= 0x10;
		if (keystate[SDL_SCANCODE_DOWN])
			input |= 0x20;
		if (keystate[SDL_SCANCODE_LEFT])
			input |= 0x40;
		if (keystate[SDL_SCANCODE_RIGHT])
			input |= 0x80;

		nes.set_input(0, input);
		nes.frame();

		SDL_QueueAudio(audio, nes.get_audio(), 1600);
		SDL_UpdateTexture(texture, NULL, nes.get_video(), WIDTH * sizeof(uint32_t));
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

	SDL_CloseAudioDevice(audio);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyTexture(texture);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
