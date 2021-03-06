#include "Player.h"
#include "Skull.h"
#include "Spawner.h"
#include "../Weapon/SpoonGun.h"
#include "../../../auxiliary/Config.h"
#include "../../../auxiliary/Time.h"
#include "../../definitions.h"
#include <iostream>
#include <cmath>

Player::Player() : Entity()
{
	classname = "player";
	team = Team::Player;
	moveType = MovementType::Walk;

	position.x = 32.0f;
	position.y = 100.0f;
	position.z = 160.0f;

	cam_rot.x = 3.14f;
	cam_rot.y = -0.3f;

	cmodel = CollisionModel(CollisionModel::ModelType::Box, glm::vec3(24, 64, 24));
	solid = true;

	lastJumpTime = 0;
	attack_finished = Time::GetCurrentTimeMillis();
	weapon = new SpoonGun(this);
	pmove = new PMoveController(this);
}

Player::~Player()
{
	delete weapon;
}

void Player::SendEntity()
{

}

void Player::Update(float deltaTime)
{
	ProjectView();
	HandlePlayerInput(deltaTime);

	HandlePMove(deltaTime);

	this->pmove->Update(deltaTime, velocity);

	glm::vec3 newPos = this->pmove->GetPosition();
	glm::vec3 expectedPosition = position + velocity;

	//this->velocity = newPos - expectedPosition;
	this->position = newPos;
}

void Player::Touch(Entity *other)
{
	//if(other->team == Team::Monster)
		//std::cout << "u dieded" << std::endl;
}

void Player::Hurt(Entity *attacker, float dmg)
{

}

void Player::Die()
{

}

void Player::ProjectView()
{
    dir = glm::vec3(cos(cam_rot.y) * sin(cam_rot.x), 
        sin(cam_rot.y),
        cos(cam_rot.y) * cos(cam_rot.x)
    );
	rotation = dir;
    right = glm::vec3(sin(cam_rot.x - 3.14f/2.0f), 0, cos(cam_rot.x - 3.14f/2.0f));

    up = glm::cross(right, dir);
}

void Player::HandlePlayerInput(float deltaTime)
{
	cam_rot.x -= input->GetMouseX() / 200.0f;
	cam_rot.y -= input->GetMouseY() / 200.0f;
	cam_rot.y = std::fmax(std::fmin(cam_rot.y, 1.4f), -1.4f);

	idir = glm::vec3(0.0f, 0.0f, 0.0f);

	glm::vec3 moveDir = glm::vec3(sin(cam_rot.x), 0.0f, cos(cam_rot.x));
	if(moveType == MovementType::Fly)
		moveDir = dir;

	if(input->KeyDown(Config::GetKey("moveForward")))
		idir += moveDir;
	else if(input->KeyDown(Config::GetKey("moveBackward")))
		idir -= moveDir;

	if(input->KeyDown(Config::GetKey("moveLeft")))
		idir -= right;
	else if(input->KeyDown(Config::GetKey("moveRight")))
		idir += right;

	if(input->KeyDown(Config::GetKey("jump")))
		idir.y = 1;

	if(input->MouseDown(1)) // FIXME this is ugly and should be written properly
	{
		weapon->PrimaryFire();
	}
	if (input->MouseDown(3)) // FIXME this is ugly and should be written properly
	{
		weapon->SecondaryFire();
	}
	// :D
	if(input->KeyDown(SDLK_LALT) && input->KeyDown(SDLK_F4))
	{
		moveType = (moveType == MovementType::Walk ? MovementType::Fly : MovementType::Walk);
		velocity = glm::vec3(0, 0, 0);
	}

	if(input->KeyPressed(SDLK_b))
		arena->SetDrawBBoxes();

	if(input->KeyPressed(SDLK_v)){	//	Skull spawning cheat
		Skull * s = new Skull();
		arena->AddEntity(s);
	}
	if(input->KeyPressed(SDLK_c)){	//	Skull spawning cheat
		Spawner * s = new Spawner();
		arena->AddEntity(s);
	}
}

void Player::HandlePMove(float deltaTime)
{
	const float ACCEL_SPEED = 12.0f;
	const float AIR_C = 20.0f;
	const float MAX_SPEED = 1.5f;
	const float MAX_AIR_SPEED = 1.75f;
	const float JUMP_HEIGHT = 1.5f;
	const float gravity = 4;
	const float airFriction = 10;
	const float friction = 5;
	const float termVel = 3;

	//onFloor = cmodel.OnFloor(position, arena->GetCModel());
	onFloor = this->pmove->OnFloor();

	/* Calculate intended movement direction */
	glm::vec3 idirxzn = glm::vec3(0, 0, 0);
	if(glm::length(glm::vec3(idir.x, 0, idir.z)) > 0)
		idirxzn = glm::vec3(idir.x, 0, idir.z);

	if(moveType == MovementType::Walk)
	{
		glm::vec3 xzVel = glm::vec3(velocity.x, 0, velocity.z);
		glm::vec3 xzVelN = glm::vec3(0, 0, 0);
		if(glm::length(xzVel) != 0)
			xzVelN = glm::normalize(xzVel);

		/* If airbound, apply airFriction and gravity */
		if(!onFloor)
		{
			/* Air friction, gravity */
			velocity -= (deltaTime * xzVelN * airFriction);
			velocity.y -= deltaTime * gravity;

			if(velocity.y < -termVel)
				velocity.y = -termVel;

			/* Air Control */
			velocity += (idirxzn * AIR_C * deltaTime);

			// Clamp speed
			if(glm::length(velocity) > MAX_AIR_SPEED)
				velocity = glm::normalize(velocity) * MAX_AIR_SPEED; // todo don't hardclamp, just add more friction
		}
		else // On Floor
		{
			if(velocity.y < 0) // Stop ent going through floor
				velocity.y = 0;

			if(glm::length(xzVelN) > 0 && glm::dot(idirxzn, xzVelN) < 0)
				velocity -= deltaTime * xzVelN * (friction*2);

			velocity -= (deltaTime * xzVelN * friction);
			velocity += (idirxzn * ACCEL_SPEED * deltaTime);

			// Clamp speed
			if(glm::length(velocity) > MAX_SPEED)
				velocity = glm::normalize(velocity) * MAX_SPEED;

			// Apply vertical velocity (jumping)
			if(idir.y > 0 && Time::GetCurrentTimeMillis() > lastJumpTime + 300)
			{
				lastJumpTime = Time::GetCurrentTimeMillis();
				velocity.y = idir.y * JUMP_HEIGHT;
				onFloor = false;
			}
		}
	}
	else if(moveType == MovementType::Fly)
	{
		const float flySpeed = 5000;
		if(glm::length(idir) > 0)
			velocity += glm::normalize(idir) * flySpeed * deltaTime;

		if(glm::length(velocity) > 0)
			velocity -= (deltaTime * glm::normalize(velocity) * friction);

		if(glm::length(velocity) > (MAX_AIR_SPEED*2))
			velocity = glm::normalize(velocity) * (MAX_AIR_SPEED*2);
	}

	/* Apply velocity to position */
	//position += velocity * deltaTime;
}