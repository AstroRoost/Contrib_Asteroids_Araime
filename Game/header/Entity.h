#pragma once
// PCH.h : #include <SFML/Graphics.hpp>
#include "PCH.h"
#include "Animation.h"

class Entity
{
public:
	float xcor = 0.f;
	float ycor = 0.f;
	float dx = 0.f;
	float dy = 0.f;
	float rad = 0.f;
	float angle = 0.f;
	bool isAlive;
	std::string name;
	Animation anim;

	Entity();

	void SetParams(Animation& a, float x, float y, float Angle = 0.f, float radius = 1.f);

	virtual void Update() {};

	// rotate and draw the sprite 
	void Draw(sf::RenderWindow& win);

	virtual ~Entity() {};
};