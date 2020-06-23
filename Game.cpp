#include "pch.h"
#include "Game.h"
#include "EntityManager.h"


const float Game::PlayerSpeed = 150.f;
const sf::Time Game::TimePerFrame = sf::seconds(1.f / 60.f);
const float Game::PlayerMissileSpeed = 500;
const float Game::BackgroundSpeed = 0.1;

Game::Game()
	: mWindow(sf::VideoMode(1200, 720), "Zaxxon", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	//mWindow.setFramerateLimit(160);

	mTexture.loadFromFile("Media/Texture/spaceship.png");
	mTBackground.loadFromFile("Media/Texture/sky.jpg");
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
}

void Game::run()
{
	sf::Clock clock;
	sf::Time timeSinceLastUpdate = sf::Time::Zero;


	while (mWindow.isOpen())
	{
		sf::Time elapsedTime = clock.restart();
		timeSinceLastUpdate += elapsedTime;
		while (timeSinceLastUpdate > TimePerFrame)
		{
			timeSinceLastUpdate -= TimePerFrame;
			processEvents();
			update(TimePerFrame);
		}
		// updateStatistics(elapsedTime);
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

void Game::update(sf::Time elapsedTime)
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

		entity->m_sprite.move(movement * elapsedTime.asSeconds());
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