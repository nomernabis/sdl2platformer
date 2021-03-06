#include "GOPlayer.h"

#include <math.h>
#include "GOConsumable.h"
#include "GameContext.h"

GOPlayer::GOPlayer(GameContext * context, Rect frame) : GameObject(context, frame)
{
	physics = new PhysicsState(this);
	physics->gravity = true;
	physics->still = false;
	originalSize = frame.size;
	health = 100;
}

void GOPlayer::handleEvent(SDL_Event * e)
{
	if (e->type == SDL_KEYDOWN)
	{
		switch (e->key.keysym.sym)
		{
		case SDLK_g:
			physics->gravity = !physics->gravity;
			if (!physics->gravity)
			{
				jumped = true;
				physics->velocity *= 0;
			}
			break;
		}
	}
	GameObject::handleEvent(e);
}

void GOPlayer::handleKeyboard(const Uint8 * state)
{
    if (!dead)
    {
        bool sitDown = false;
        bool moveLeft = false;
        bool moveRight = false;
        Vector2D moveVector = Vector2D();
        if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_A])
        {
            moveVector += Vector2D(-speed, 0);
            moveLeft = true;
        }
        if (state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_D])
        {
            moveVector += Vector2D(speed, 0);
            moveRight = true;
        }
        if (state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_W] || state[SDL_SCANCODE_SPACE])
        {
            if (!physics->gravity)
            {
                moveVector += Vector2D(0, -speed);
            }
            else
            {
                if (!jumped)
                {
                    physics->velocity += Vector2D(0, -jumpSpeed);
                    jumped = true;
                }
            }
        }
        if (state[SDL_SCANCODE_DOWN] || state[SDL_SCANCODE_S] || state[SDL_SCANCODE_LCTRL] || state[SDL_SCANCODE_RCTRL])
        {
            if (!physics->gravity)
            {
                moveVector += Vector2D(0, speed);
            }
            else
            {
                sitDown = true;
            }
        }
        setCrouched(sitDown);

        if (moveLeft && !moveRight)
        {
            moveAnimation->turnLeft(true);
            crouchAnimation->turnLeft(true);
            crouchMoveAnimation->turnLeft(true);
        }
        if (moveRight && !moveLeft)
        {
            moveAnimation->turnLeft(false);
            crouchAnimation->turnLeft(false);
            crouchMoveAnimation->turnLeft(false);
        }

        if (!moveLeft && !moveRight && !jumped && !crouched)
            animation = idleAnimation;
        if (!moveLeft && !moveRight && !jumped && crouched)
            animation = crouchAnimation;
        if ((moveLeft || moveRight) && !jumped && !crouched)
            animation = moveAnimation;
        if ((moveLeft || moveRight) && !jumped && crouched)
            animation = crouchMoveAnimation;
        if (jumped && crouched)
            animation = crouchAnimation;
        if (jumped && !crouched)
            animation = jumpAnimation;

        frame.center.x += moveVector.x;
        frame.center.y += moveVector.y;
    }
	GameObject::handleKeyboard(state);
}

void GOPlayer::handleEnterCollision(Collision collision)
{
	if (GOConsumable * consumable = dynamic_cast<GOConsumable *>(collision.collider))
	{
	    power += 1;
	    context->ui->powerBar->setValue(power);
		consumable->removed = true;
		speed += 0.01f;
		jumpSpeed += 0.01f;
	    if (power > 99)
        {
            win();
        }
	}
}

void GOPlayer::handleExitCollision(GameObject * collider)
{
	if (!physics->colliders.size())
	{
		jumped = true;
	}
}

void GOPlayer::handleCollision(Collision collision)
{
	if (fabs(collision.collisionVector.x) > fabs(collision.collisionVector.y))
	{
		if (collision.collisionVector.y > 0 && jumped && physics->gravity)
		{
			jumped = false;
		}
	}
}

void GOPlayer::dealDamage(int damage)
{
    if (!won)
    {
        health -= damage;
        context->ui->healthBar->setValue(health);
        if (health < 0)
        {
            die();
        }
    }
}

void GOPlayer::die()
{
	dead = true;
	context->ui->deathText->visible = true;
}

void GOPlayer::win()
{
    won = true;
    context->ui->winText->visible = true;
}

void GOPlayer::free()
{
	if (idleAnimation)
		idleAnimation->free();
	if (moveAnimation)
		moveAnimation->free();
	if (jumpAnimation)
		jumpAnimation->free();
	if (crouchAnimation)
		crouchAnimation->free();
	if (crouchMoveAnimation)
		crouchMoveAnimation->free();

	renderObject = NULL;
	animation = NULL;
	GameObject::free();
}

void GOPlayer::setCrouched(bool crouched)
{
	if (crouched && !this->crouched)
	{
		this->crouched = true;
		frame.size.height = originalSize.height / 2;
		frame.center.y += frame.size.height / 2;
	}
	else if (!crouched && this->crouched)
	{
		this->crouched = false;
		frame.center.y -= frame.size.height / 2;
		frame.size.height = originalSize.height;
	}
}

