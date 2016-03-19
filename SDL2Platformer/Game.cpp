#include "Game.h"

#include "RenderObject.h"
#include "GOPlayer.h"
#include "GOFrame.h"

bool Game::init()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	GameSettings * settings = new GameSettings();
	context = new GameContext(settings);
	return context != NULL;
}

void Game::run()
{
	if (init())
	{
		SDL_Event e;

		context->world = new GOWorld(context, Rect(0, 0, context->settings->windowWidth/2, context->settings->windowHeight/2));
		context->world->camera->originalSize.width /= 2;
		context->world->camera->originalSize.height /= 2;

		GOPlayer * player = new GOPlayer(context, Rect( 0,20,10,20 ));
		player->renderObject = new RenderObject(context, player, Color(0x00, 0xFF, 0x00, 0xFF));
		player->speed = 4;
		player->jumpSpeed = 10;
		player->addChild(context->world->camera);

		context->world->addChild(player);

		context->world->addChild(new GOFrame(context, Rect( 0,0,context->world->frame.size.width,context->world->frame.size.height ), 10));

		GOSolid * brick;
		for (int i = 0; i < 3; i++)
		{
			brick = new GOSolid(context, Rect( -context->world->frame.size.width/2 + 15 + i * 10, context->world->frame.size.height/2 - 15, 10, 10 ));
			brick->renderObject = new RenderObject(context, brick, Color(0x00, 0x00, 0x00, 0xFF));
			context->world->addChild(brick);
		}

		//brick = NULL;
		player = NULL;

		while (!context->quit)
		{
			//Handle events on queue
			while (SDL_PollEvent(&e) != 0)
			{
				switch (e.type)
				{
				case SDL_QUIT:
					context->quit = true;
					break;
				}
				//player->handleEvent(&e);
				context->world->handleEvent(&e);
			}

			context->world->handleKeyboard(SDL_GetKeyboardState(NULL));
			context->world->processPhysics();
			context->world->detectCollisions();

			//Clear screen
			SDL_SetRenderDrawColor(context->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(context->renderer);

			context->world->render();

			//Update screen
			SDL_RenderPresent(context->renderer);
		}
	}
	exit();
}

void Game::exit()
{
	context->free();

	SDL_Quit();
}