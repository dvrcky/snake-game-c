#include <SDL2/SDL.h>
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_video.h"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <stdio.h>

/* Display properties */
#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 800

/* Snake`s properties */
#define START_X 250
#define START_Y 200
#define SNAKE_W 25
#define SNAKE_H 25

/* Colours */
#define COLOR_BLACK 0x000000
#define COLOR_RED 0xff0000
#define COLOR_GREEN 0x00ff00

static bool is_running = true;

static int8_t dir_x = 1;
static int8_t dir_y = 0;

static char previous_button = 'd';
static bool is_changed_dir = false;

typedef struct Block {
	SDL_Rect rect;
	bool is_exist;
} Block;

typedef struct Snake {
 	Uint16 score;
	Block* blocks;
	Uint32 color;
	Uint16 size;
} Snake;

void eat_apple(Snake* snake) {
	snake->blocks[snake->size++].is_exist = true;

	if (previous_button == 'd') {
		snake->blocks[snake->size - 1].rect = (SDL_Rect) {
			.x = snake->blocks[snake->size - 2].rect.x - SNAKE_W,
			.y = snake->blocks[snake->size - 2].rect.y,
		};
	}

	if (previous_button == 'a') {
		snake->blocks[snake->size - 1].rect = (SDL_Rect) {
			.x = snake->blocks[snake->size - 2].rect.x + SNAKE_W,
			.y = snake->blocks[snake->size - 2].rect.y,
		};
	}

	if (previous_button == 's') {
		snake->blocks[snake->size - 1].rect = (SDL_Rect) {
			.x = snake->blocks[snake->size - 2].rect.x,
			.y = snake->blocks[snake->size - 2].rect.y - SNAKE_H,
		};
	}

	if (previous_button == 'w') {
		snake->blocks[snake->size - 1].rect = (SDL_Rect) {
			.x = snake->blocks[snake->size - 2].rect.x,
			.y = snake->blocks[snake->size - 2].rect.y + SNAKE_H,
		};
	}

	snake->blocks[snake->size - 1].rect.h = snake->blocks[snake->size - 1].rect.w = SNAKE_W;
}

void move_snake(Snake* snake, SDL_Surface* surface) {
	Uint16 previous_x = snake->blocks[0].rect.x;
	Uint16 previous_y = snake->blocks[0].rect.y;

	switch (previous_button) {
		case 'w': {
			snake->blocks[0].rect.y -= SNAKE_H;
			break;
		}

		case 's': {
			snake->blocks[0].rect.y += SNAKE_H;
			break;
		}

		case 'a': {
			snake->blocks[0].rect.x -= SNAKE_W;
			break;
		}

		case 'd': {
			snake->blocks[0].rect.x += SNAKE_W;
			break;
		}
	}

	SDL_FillRect(surface, &snake->blocks[0].rect, snake->color);


	for (size_t i = 1; i < snake->size; ++i) {
		Uint16 temp_x = snake->blocks[i].rect.x;
		Uint16 temp_y = snake->blocks[i].rect.y;

		snake->blocks[i].rect.x = previous_x;
		snake->blocks[i].rect.y = previous_y;

		previous_x = temp_x;
		previous_y = temp_y;

		SDL_FillRect(surface, &snake->blocks[i].rect, snake->color);
	}
}

bool is_smashed(Snake* snake) {
	if (snake->blocks[0].rect.x >= SCREEN_WIDTH || snake->blocks[0].rect.x <= 0 ||
		snake->blocks[0].rect.y >= SCREEN_HEIGHT || snake->blocks[0].rect.y <= 0) return true;

	for (size_t i = 1; i < snake->size; ++i) {
		if (snake->blocks[0].rect.x == snake->blocks[i].rect.x &&
			snake->blocks[0].rect.y == snake->blocks[i].rect.y)
			return true;
	}

	return false;
}

void restart(Snake* snake, SDL_Rect* apple) {
	snake->blocks[0].rect.x = START_X;
	snake->blocks[0].rect.y = START_Y;

	for (size_t i = 1; i < snake->size; ++i) {
		snake->blocks[i].is_exist = false;
	}

	snake->size = 1;

	apple->x = rand() % 40 * 25;
	apple->y = rand() % 32 * 25;

	return;
}

int main() {
	/*
	 *
	 * Several required initializations.
	 *
	 * */
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return 1;

	SDL_Window* window = SDL_CreateWindow("Snake Game",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT, 0);
	if (window == NULL) return 1;

	SDL_Surface* surface = SDL_GetWindowSurface(window);

	SDL_FillRect(surface, NULL,
		SDL_MapRGB(surface->format,  0, 0, 0));

	SDL_UpdateWindowSurface(window);
	SDL_Event event;

	Snake snake = (Snake) {
		.blocks = calloc(30, sizeof(Block)),
		.color = COLOR_GREEN,
		.size = 1,
	};

	snake.blocks[0].rect.x = START_X;
	snake.blocks[0].rect.y = START_Y;
	snake.blocks[0].rect.w = SNAKE_W;
	snake.blocks[0].rect.h = SNAKE_H;
	snake.blocks[0].is_exist = true;

	SDL_Rect apple = (SDL_Rect) {
		.x = rand() % 40 * 25,
		.y = rand() % 32 * 25,
		.w = SNAKE_W,
		.h = SNAKE_H,
	};

	while (is_running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) { is_running = false; }
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym) {
					case SDLK_d:
						if (previous_button != 'a' || snake.size == 1) {
							dir_x = 1; dir_y = 0;
							previous_button = 'd';
							is_changed_dir = true;
						}
						break;

					case SDLK_w:
						if (previous_button != 's' || snake.size == 1) {
							dir_y = -1; dir_x = 0;
							previous_button = 'w';
							is_changed_dir = true;
						}
						break;

					case SDLK_s:
						if (previous_button != 'w' || snake.size == 1) {
							dir_y = 1; dir_x = 0;
							previous_button = 's';
							is_changed_dir = true;
						}
						break;

					case SDLK_a:
						if (previous_button != 'd' || snake.size == 1) {
							dir_x = -1; dir_y = 0;
							previous_button = 'a';
							is_changed_dir = true;
						}
						break;
				}
			}
		}

		SDL_FillRect(surface, NULL,
			SDL_MapRGB(surface->format,  0, 0, 0));
		SDL_FillRect(surface, &apple, COLOR_RED);
		move_snake(&snake, surface);

		if (snake.blocks[0].rect.x == apple.x &&
			snake.blocks[0].rect.y == apple.y) {
			eat_apple(&snake);
			apple.x = rand() % 40 * 25;
			apple.y = rand() % 32 * 25;
		}

		SDL_UpdateWindowSurface(window);

		if (is_smashed(&snake)) { restart(&snake, &apple); }

		SDL_Delay(100);

		is_changed_dir = false;
	}

	SDL_DestroyWindow(window);

	SDL_Quit();

	return EXIT_SUCCESS;
}
