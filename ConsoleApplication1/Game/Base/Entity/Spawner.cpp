#include <iostream>
#include <List>

#include <glm/gtc/type_ptr.hpp>

#include "../../../auxiliary/Time.h"
#include "../../definitions.h"
#include "Spawner.h"
#include "Player.h"
#include "Skull.h"
#include "../Arena.h"

extern Player *player;


Spawner::Spawner() : Monster()
{
	modelname = "spawner";
	classname = "spawner_1";
	health = 400;
	speed = 10.0f;
	moveType = MovementType::Fly;
	cmodel = CollisionModel(CollisionModel::ModelType::Sphere, 14);
	solid = true;
	spawn_timer = (float)(Time::GetCurrentTimeMillis() + 10000 + 500*rand()/RAND_MAX);

	float center_distance = 500.0f;
	float theta = 6.28f*rand()/RAND_MAX;
	position = glm::vec3(center_distance*cos(theta), 60.0f, center_distance*sin(theta));
	velocity = normalize(glm::vec3(0,60.0f,0) - position)*speed;
}

void Spawner::SpawnSkulls(int amount){
	while(amount-- > 0){
		Skull * s = new Skull();
		s->position = position;

		float theta = 6.28f*rand()/RAND_MAX;
		float r = (float)(rand()/RAND_MAX);
		s->velocity = 80.0f * glm::vec3(cos(theta)*r, 3.5f+rand()/RAND_MAX, sin(theta) * r);
		arena->AddEntity(s);
	}
};

void Spawner::Update(float deltaTime)
{
	if (spawn_timer < Time::GetCurrentTimeMillis()) {
		SpawnSkulls(8);
		spawn_timer = (float)(Time::GetCurrentTimeMillis() + 10000 + 500 * rand() / RAND_MAX);
	}

}

Spawner::~Spawner()
{
;
}

void Spawner::Touch(Entity *other)
{
	
}

void Spawner::Hurt(Entity *attacker, float dmg)
{
	if(attacker->team != team)
		Die();
}

void Spawner::Die()
{
	destroy = true;
}