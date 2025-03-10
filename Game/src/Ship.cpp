#include "../header/Ship.h"
#include "../header/Game.h"

Ship::Ship()
{
	name = "player";
}

void Ship::Update()
{
	// Used for Shift Speed: boostFactor
	// Boost factor for increased speed 
	// Able to "semi-somewhat : Tokyo drift", if shift key is let go while going in a certain angle
	float speed = 1.5f;  // Boosted speed factor
	float normal = 1.0f; // Normal speed factor	
	float boostFactor = isBoosting ? speed : normal;

	if (isAccelerating) {
		dx += cos(angle * DEGTORAD) * SHIP_ACCELERATION * boostFactor;
		dy += sin(angle * DEGTORAD) * SHIP_ACCELERATION * boostFactor;
	}
	else if (isDeaccelerating) {
		// Apply deacceleration
		dx *= DECELERATION_FACTOR;
		dy *= DECELERATION_FACTOR;
		// If speed is very low or negative (going backward), invert the direction
		if (sqrt(dx * dx + dy * dy) < 0.1f) {
			dx -= cos(angle * DEGTORAD) * SHIP_ACCELERATION;
			dy -= sin(angle * DEGTORAD) * SHIP_ACCELERATION;
		}
	}
	else {
		// Normal friction deacceleration
		dx *= DECELERATION_FACTOR;
		dy *= DECELERATION_FACTOR;
	}
	

	//float speed = sqrt(dx * dx + dy * dy);
	float currentSpeed = sqrt(dx * dx + dy * dy);
	float maxAllowedSpeed = isBoosting ? maxSpeed * boostFactor : maxSpeed;
	if (currentSpeed > maxAllowedSpeed) {
		dx *= maxAllowedSpeed / currentSpeed;
		dy *= maxAllowedSpeed / currentSpeed;
	}
	//if (speed > maxSpeed)
//	{
	//	dx *= maxSpeed / speed;
	//	dy *= maxSpeed / speed;
	//}

	xcor += dx;
	ycor += dy;
	


	// check if it's beyond the edge of the screen
	if (xcor > SCREEN_WIDTH)
	{
		xcor = 0.f;
	}
	if (xcor < 0)
	{
		xcor = SCREEN_WIDTH;
	}
	if (ycor > FIELD_HEIGHT)
	{
		ycor = 0.f;
	}
	if (ycor < 0)
	{
		ycor = FIELD_HEIGHT;
	}
}

void Ship::HandlePlayerInput(Game& game)
{
	switch (game.gameState)
	{
	case GameState::Menu:
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
		{
			game.RestartGame(game);
		}
		break;
	}
	case GameState::Game:
	{
		if (game.player->isDestroyed) return;

		// update current time
		game.newTime = game.gameTimer.getElapsedTime().asSeconds();
		// Shift Speed
		// Check if shift key is pressed for boosting
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
			isBoosting = true;
		}
		else {
			isBoosting = false;
		}

		// A & D  added to keys to move right/left
		// handle rotation
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || (sf::Keyboard::isKeyPressed(sf::Keyboard::D)))
		{
			game.player->angle += ROTATION_SPEED;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || (sf::Keyboard::isKeyPressed(sf::Keyboard::A)))
		{
			game.player->angle -= ROTATION_SPEED;
		}

		// make shot
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			MakeShot(game, game.player->rocketX);
		}

		// equip rocket
		if (game.player->weapon == Weapon::Laser)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
			{
				game.player->weapon = Weapon::Rocket;

				// change highlighter coord
				game.UI.UpdateWeaponHighlighterPos(SELECT2_XCOR, SELECT_YCOR);

				// play weapon change sound
				game.weapChangeSnd.sound.play();
			}
		}

		// equip laser
		if (game.player->weapon == Weapon::Rocket)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
			{
				game.player->weapon = Weapon::Laser;

				// change highlighter coord
				game.UI.UpdateWeaponHighlighterPos(SELECT1_XCOR, SELECT_YCOR);

				// play weapon change sound
				game.weapChangeSnd.sound.play();
			}
		}

		// W & S keys are added for going forward, or stopping then "reversing", some addition for being bit slow going backward
		// "due to not using main thrust, and just using the ship boosters" : some lore ship logic
		// check if ship is accelerated
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			isAccelerating = true;
			isDeaccelerating = false; // Ensure we're not deaccelerating while accelerating	
		
		// check if ship is deaccelerating
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
			isDeaccelerating = true;
			isAccelerating = false; // Ensure we're not accelerating while deaccelerating
		}
		else {
			isAccelerating = false;
			isDeaccelerating = false;
		}
		
		break;
	}
	case GameState::GameOver:
	{
		break;
	}
	default:
		break;
	}
}

void Ship::MakeShot(Game& game, float xcor)
{
	// ALARM , the pilot will will be filled with adrenaline, ship is going down/ prob will die / injected with stims to keep going
	// Pilot will shoot more aggressivly / promotes a bit more aggresive play style
	// Modify the cooldown based on player's health
	// Speed up cooldown if health is below 50 , 1.0f is default speed
	float attackSpeedMultiplier = (game.player->health <= 50 && game.player->health > 0) ? 0.6f : 1.0f; 

	switch (game.player->weapon)
	{
	case Weapon::Laser:
	{
		// check if enough time has passed, adjusting for health-based attack speed
		if (game.newTime - game.pastTime > LASER_COOLDOWN * attackSpeedMultiplier)
		{
			// create new laser
			Laser* laser = new Laser();

			// Random angle variation for laser shot - adds a slight inaccuracy to each shot
			float rand_angle = static_cast<float>(rand() % 6 - 3);
			laser->SetParams(game.sLaser, game.player->xcor, game.player->ycor,
				game.player->angle + rand_angle, LASER_RAD);
			game.entities.push_back(laser);

			// play random laser sound
			game.laserSndArray[rand() % LASER_SND_QTY].sound.play();

			// update timer
			// POWERUP IDEA: IF disabled, laser will spam as a machine gun for a short time frame
			game.pastTime = game.newTime;
		}
		break;
	}
	case Weapon::Rocket:
	{
		// check if enough time has passed, adjusting for health-based attack speed
		if (game.newTime - game.pastTime > ROCKET_COOLDOWN * attackSpeedMultiplier)
		{
			for (int i = 0; i < ROCKETS_QTY; i++)
			{
				// create new rocket
				Rocket* rocket = new Rocket();
				rocket->SetParams(game.sRocket, game.player->xcor, game.player->ycor,
					game.player->angle - xcor, ROCKET_RAD);
				game.entities.push_back(rocket);
				xcor += ROCKET_ANGLE_STEP;
			}

			// play random rocket sound
			game.rocketSndArray[rand() % ROCKETS_SND_QTY].sound.play();

			// update timer
			// POWERUP IDEA: IF disabled, missile will spam for a short time frame
			game.pastTime = game.newTime;
		}
		break;
	}
	default:
		break;
	}
}



void Ship::UpdateShipSprite(Game& game)
{
	if (!game.player->isDestroyed)
	{
		// change sprite if ship is accelerated
		if (game.player->isAccelerating)
		{
			game.player->anim = game.sFlyingShip;
		}
		else if (game.player->isDeaccelerating)
		{
			game.player->anim = game.sShip;
		}
	else
	{
		game.player->anim = game.sShip;
	}
	}
}

void Ship::TakeDamage(Game& game, const float damage)
{
	game.player->health -= damage * DAMAGE_MULTIPLIER;

	if (game.player->health < 0.f)
	{
		game.player->health = 0.f;
	}

	// update player health
	game.UI.UpdateUIHealthBar(game.player->health);

	if (game.player->health == 0.f)
	{
		game.player->isDestroyed = true;
		game.player->ships--;
		game.shipExplSnd.sound.play();
	}
}

void Ship::RestartPlayer(Game& game)
{
	if (!game.player->isDestroyed) return;

	// update current time
	game.newTime = game.gameTimer.getElapsedTime().asSeconds();

	if (game.destroy_cooldown)
	{
		if (game.newTime - game.pastTime > COUNTER)
		{
			// update cooldown counter
			game.destroy_cooldown -= COUNTER;
			game.timerSnd.sound.play();

			// update cooldown text
			game.cooldownText.UpdateText(game.cooldownStr + std::to_string(game.destroy_cooldown));

			// update past time
			game.pastTime = game.newTime;
		}
	}
	if (!game.destroy_cooldown)
	{
		// ressurect the player
		float angle = 0.f;

		game.player->SetParams(game.sShip, static_cast<float>(SCREEN_WIDTH / 2),
							   static_cast<float>(FIELD_HEIGHT / 2), angle, SHIP_RAD);
		game.player->dx = 0.f;
		game.player->dy = 0.f;
		game.player->isAccelerating = false;
		game.player->health = SHIP_HEALTH;
		game.player->isDestroyed = false;

		// update player health
		game.UI.UpdateUIHealthBar(game.player->health);
	}
}