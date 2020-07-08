#include "pch.h"
#include "Game.h"
#include "EntityManager.h"


const float Game::PlayerSpeed = 5.f;
const float Game::PlayerMissileSpeed = 20;
const float Game::BackgroundSpeed = 0.1;
const float Game::EnemySpeed = 0.5f;
const float Game::EnemyMissilesSpeed = 10.0f;
int Game::EnemyMissilesNumber = 0;



Game::Game()
	: mWindow(sf::VideoMode(1280, 720), "Zaxxon", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	mWindow.setFramerateLimit(160);

	mTexture.loadFromFile("Media/Texture/spaceship.jpg");
	mTBackground.loadFromFile("Media/Texture/sky.jpg");
	mTEnemy.loadFromFile("Media/Texture/espaceship.png");

	InitSprites();
}

void Game::ResetSprites()
{
    //_IsGameOver = false;
	//_IsEnemyWeaponFired = false;
	_IsPlayerWeaponFired = false;
	//_IsEnemyMasterWeaponFired = false;
	mBackground.setPosition(mBackground.getOrigin());

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		entity->m_enabled = true;
	}
}


void Game::InitSprites()
{

	_IsPlayerWeaponFired = false;

	// Player
	mPlayer.setTexture(mTexture);
	mPlayer.setPosition(10.f, 250.f);

	mPlayer.scale(0.1,0.1);

	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = mTexture.getSize();
	player->m_position = mPlayer.getPosition();
	mBackground.setTexture(mTBackground);
	mBackground.scale(0.5, 0.5);
	EntityManager::m_Entities.push_back(player);


	// Enemies
	float ecart = mWindow.getSize().x;
	std::srand(time(0));
	for (int j = 0; j < 8; j++) {

		for (int i = 0; i < 6; i++)
		{
			mEnemy[i].setTexture(mTEnemy);
			mEnemy[i].setPosition(
				mWindow.getSize().x + (ecart * i) + std::rand() % 300,
				std::rand() % int(mWindow.getSize().y - mEnemy[i].getTexture()->getSize().y * mEnemy[i].getScale().y)
				);
			std::shared_ptr<Entity> se = std::make_shared<Entity>();
			se->m_sprite = mEnemy[i];
			se->m_type = EntityType::enemy;
			se->m_size = mTEnemy.getSize();
			se->m_position = mEnemy[i].getPosition();
			EntityManager::m_Entities.push_back(se);
		}
	}

	// Enemy Missile
	mEnemyMissile.setTexture(mTEnemyMissile);

}

void Game::run()
{

	while (mWindow.isOpen())
	{
		processEvents();
		update();

		render();
	}
}


void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event))
	{
		switch (event.type)
		{
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			break;

		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;

		case sf::Event::Closed:
			mWindow.close();
			break;
		}
	}
}

void Game::update()
{
	float bgMove = mBackground.getPosition().x <= -float(mTBackground.getSize().x - 10) ? 0 : BackgroundSpeed;
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		sf::Vector2f movement(0.f, 0.f);
		if (entity->m_enabled == false)
		{
			continue;
		}
		if (entity->m_type == EntityType::player)
		{
			if (mIsMovingUp && entity->m_sprite.getPosition().y > 0)
				movement.y -= PlayerSpeed;
			if (mIsMovingDown && entity->m_sprite.getPosition().y < mWindow.getSize().y - (mTexture.getSize().y * entity->m_sprite.getScale().y))
				movement.y += PlayerSpeed;
			if (mIsMovingRight && entity->m_sprite.getPosition().x < mWindow.getSize().x)
				movement.x += PlayerSpeed;
			if (mIsMovingLeft && entity->m_sprite.getPosition().x > 0 + (mTexture.getSize().x * entity->m_sprite.getScale().x))
				movement.x -= PlayerSpeed;
		}
		else if (entity->m_type == EntityType::weapon)
		{
			movement.x = PlayerMissileSpeed;
			if (entity->m_sprite.getPosition().x > mWindow.getSize().x) {
				entity->m_enabled = false;
			}
			_IsPlayerWeaponFired = false;
		}
		else if (entity->m_type == EntityType::enemy)
		{
			movement.x = -((double)std::rand() / (RAND_MAX)+2) * EnemySpeed;
			if (Game::EnemyMissilesNumber <= 2 && entity->m_sprite.getPosition().x < 1000) {
				std::shared_ptr<Entity> ew = std::make_shared<Entity>();
				mTEnemyMissile.loadFromFile("Media/Texture/elaser.png");
				ew->m_sprite.setTexture(mTEnemyMissile);
				ew->m_sprite.setScale(0.5, 0.5);

				ew->m_size = mTEnemyMissile.getSize();
				ew->m_sprite.setPosition(
					entity->m_sprite.getPosition().x + (entity->m_sprite.getTexture()->getSize().x * entity->m_sprite.getScale().x) / 2,
					entity->m_sprite.getPosition().y + (entity->m_sprite.getTexture()->getSize().y * entity->m_sprite.getScale().y) / 2);
				ew->m_type = EntityType::enemyWeapon;
				EntityManager::m_Entities.push_back(ew);
				Game::EnemyMissilesNumber++;
			}
		}
		else if (entity->m_type == EntityType::enemyWeapon) {
			movement.x = -EnemyMissilesSpeed;
		}

		entity->m_sprite.move(movement);
	}

	mBackground.move(-bgMove, 0);
}

void Game::render()
{
	mWindow.clear();
	mWindow.draw(mBackground);
	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		if (entity->m_enabled == false)
		{
			continue;
		}

		mWindow.draw(entity->m_sprite);
	}


	mWindow.display();
}


void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	if (key == sf::Keyboard::Up)
		mIsMovingUp = isPressed;
	else if (key == sf::Keyboard::Down)
		mIsMovingDown = isPressed;
	else if (key == sf::Keyboard::Left)
		mIsMovingLeft = isPressed;
	else if (key == sf::Keyboard::Right)
		mIsMovingRight = isPressed;

	if (key == sf::Keyboard::Space)
	{
		if (isPressed == false)
		{
			return;
		}

		if (_IsPlayerWeaponFired == true)
		{
			return;
		}

		std::shared_ptr<Entity> sw = std::make_shared<Entity>();
		mTMissile.loadFromFile("Media/Texture/laser.png");
		sw->m_sprite.setTexture(mTMissile);
		sw->m_sprite.setScale(0.5, 0.5);
		
		sw->m_size = mTMissile.getSize();
		sw->m_sprite.setPosition(
			EntityManager::GetPlayer()->m_sprite.getPosition().x + (EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().x * EntityManager::GetPlayer()->m_sprite.getScale().x) / 2,
			EntityManager::GetPlayer()->m_sprite.getPosition().y + (EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y * EntityManager::GetPlayer()->m_sprite.getScale().y) / 2);
		sw->m_type = EntityType::weapon;
		EntityManager::m_Entities.push_back(sw);
		_IsPlayerWeaponFired = true;
	}
}