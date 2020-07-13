#include "pch.h"
#include "Game.h"
#include "EntityManager.h"


const float Game::PlayerSpeed = 5.f;
const float Game::PlayerMissileSpeed = 20;
const float Game::BackgroundSpeed = 0.1;
const float Game::EnemySpeed = 0.5f;
const float Game::EnemyMissilesSpeed = 2.0f;
int Game::BossMeteorNumber = 0;



Game::Game()
	: mWindow(sf::VideoMode(1280, 720), "Zaxxon", sf::Style::Close)
	, mTexture()
	, mPlayer()
	, mGameOver(false)
	, mVictory(false)
	, mIsMovingUp(false)
	, mIsMovingDown(false)
	, mIsMovingRight(false)
	, mIsMovingLeft(false)
{
	mWindow.setFramerateLimit(144);

	mTexture.loadFromFile("Media/Texture/spaceship.png");
	mTBackground.loadFromFile("Media/Texture/sky.jpg");
	mTEnemy.loadFromFile("Media/Texture/espaceship.png");
	mTBoss.loadFromFile("Media/Texture/eboss.png");
	mTGameOverMessage.loadFromFile("Media/Texture/gameOver.png");
	mTVictoryMessage.loadFromFile("Media/Texture/victory.png");
	lifeFont.loadFromFile("Media/Font/godeater.ttf");
	mTMeteor.loadFromFile("Media/Texture/meteor.png");

	sBLaser.loadFromFile("Media/Sound/laser.wav");
	sBExplosion.loadFromFile("Media/Sound/explosion.wav");

	sBackground.openFromFile("Media/Sound/background.wav");
	sBackground.setLoop(true);
	sBackground.setVolume(10);

	sGameover.openFromFile("Media/Sound/gameover.wav");
	sGameover.setLoop(true);
	sGameover.setVolume(20);

	sVictory.openFromFile("Media/Sound/victory.wav");
	sVictory.setLoop(true);
	sVictory.setVolume(20);

	InitSprites();
}

void Game::ResetSprites()
{
	_IsPlayerWeaponFired = false;
	mBackground.setPosition(mBackground.getOrigin());

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities)
	{
		entity->m_enabled = true;
	}
}


void Game::InitSprites()
{

	_IsPlayerWeaponFired = false;

	sBackground.play();
	sExplosion.setBuffer(sBExplosion);
	sExplosion.setVolume(10);
	sLaser.setBuffer(sBLaser);
	sLaser.setVolume(10);

	mGameOverMessage.setTexture(mTGameOverMessage);
	mGameOverMessage.setScale(2.0,2.0);
	mGameOverMessage.setPosition(mWindow.getSize().x /2 - (mTGameOverMessage.getSize().x * mGameOverMessage.getScale().x ) /2 ,
		mWindow.getSize().y / 2 - (mTGameOverMessage.getSize().y * mGameOverMessage.getScale().y) / 2);

	mVictoryMessage.setTexture(mTVictoryMessage);
	mVictoryMessage.setScale(2.0, 2.0);
	mVictoryMessage.setPosition(mWindow.getSize().x / 2 - (mTVictoryMessage.getSize().x * mVictoryMessage.getScale().x) / 2,
		mWindow.getSize().y / 2 - (mTVictoryMessage.getSize().y * mVictoryMessage.getScale().y) / 2);

	// Player
	mPlayer.setTexture(mTexture);
	mPlayer.setPosition(10.f, 250.f);
	mPlayer.scale(0.5,0.5);
	std::shared_ptr<Entity> player = std::make_shared<Entity>();
	player->m_sprite = mPlayer;
	player->m_type = EntityType::player;
	player->m_size = mTexture.getSize();
	player->m_position = mPlayer.getPosition();
	player->damage = 10;
	player->life = 50;
	EntityManager::m_Entities.push_back(player);


	mBackground.setTexture(mTBackground);
	mBackground.scale(0.5, 0.5);

	// Enemies
	float screenSize = mWindow.getSize().x;
	std::srand(time(0));
	for (int j = 0; j < 8; j++) {

		for (int i = 0; i < 6; i++)
		{
			mEnemy[i].setTexture(mTEnemy);
			mEnemy[i].setPosition(
				mWindow.getSize().x + (screenSize * i) + std::rand() % 300,
				std::rand() % int(mWindow.getSize().y - mEnemy[i].getTexture()->getSize().y * mEnemy[i].getScale().y)
				);
			std::shared_ptr<Entity> se = std::make_shared<Entity>();
			se->m_sprite = mEnemy[i];
			se->m_type = EntityType::enemy;
			se->m_size = mTEnemy.getSize();
			se->m_position = mEnemy[i].getPosition();
			se->damage = 10;
			se->life = 30;
			EntityManager::m_Entities.push_back(se);
		}
	}

	for (int j = 0; j < 8; j++) {

		for (int i = 0; i < 15; i++)
		{
			std::shared_ptr<Entity> ew = std::make_shared<Entity>();
			mTEnemyMissile.loadFromFile("Media/Texture/elaser.png");
			ew->m_sprite.setTexture(mTEnemyMissile);
			ew->m_size = mTEnemyMissile.getSize();
			ew->m_sprite.setPosition(mWindow.getSize().x + (screenSize * i) + std::rand() % 300, std::rand() % 2 == 0 ? -30 : 750);
			ew->m_type = EntityType::enemyWeapon;
			ew->damage = 10;
			ew->life = 10;
			EntityManager::m_Entities.push_back(ew);
		}
	}


	std::shared_ptr<Entity> bw = std::make_shared<Entity>();
	bw->m_sprite.setTexture(mTBoss);
	bw->m_size = mTBoss.getSize();
	bw->m_sprite.setScale(3.0, 3.0);
	bw->m_sprite.setPosition(mWindow.getSize().x + (screenSize * 2) , mWindow.getSize().y / 2 - (bw->m_sprite.getTexture()->getSize().y * bw->m_sprite.getScale().y /2 ) ) ;
	bw->m_type = EntityType::enemyMaster;
	bw->damage = 20;
	bw->life = 800;
	
	EntityManager::m_Entities.push_back(bw);
	

	// Enemy Missile
	mEnemyMissile.setTexture(mTEnemyMissile);


	// Life display
	lifeDisplay.setFont(lifeFont);
	lifeDisplay.setCharacterSize(100);
	lifeDisplay.setPosition(0, mWindow.getSize().y - lifeDisplay.getCharacterSize());

}

void Game::run()
{

	while (mWindow.isOpen())
	{
		processEvents();
		if (!mGameOver && !mVictory) {
			update();
			handleCollisions();
		}
		
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
			if (mIsMovingLeft && entity->m_sprite.getPosition().x > 0)
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


		if (entity->m_sprite.getPosition().x <= 0) {
			entity->m_enabled = false;
		}
		}
		else if (entity->m_type == EntityType::enemyMaster)
		{
			if (entity->m_sprite.getPosition().x > 900) {
				movement.x = -EnemySpeed;
			}


			if (entity->m_sprite.getPosition().x <= 0) {
				entity->m_enabled = false;
			}
		} else if (entity->m_type == EntityType::enemyWeapon) {
			movement.x = -EnemyMissilesSpeed;
			std::srand(time(0));
			if (entity->m_sprite.getPosition().x <= 1200 - std::rand() % 100) {
				if (entity->m_sprite.getPosition().y < EntityManager::GetPlayer()->m_sprite.getPosition().y + (EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y * EntityManager::GetPlayer()->m_sprite.getScale().y) / 2) {
					movement.y = EnemyMissilesSpeed;
				}
				else if (entity->m_sprite.getPosition().y > EntityManager::GetPlayer()->m_sprite.getPosition().y + (EntityManager::GetPlayer()->m_sprite.getTexture()->getSize().y * EntityManager::GetPlayer()->m_sprite.getScale().y) / 2) {
					movement.y = -EnemyMissilesSpeed;
				}
				else {
					movement.y = 0;
				}
			}
		}
		else if (entity->m_type == EntityType::enemyMasterWeapon) {
			movement.x = - ((entity->time + pow(cos(entity->time), 3) - 3 * pow(sin((entity->time)), 2)) *0.5);
			movement.y = -(3 * sin(entity->time) * (2 * cos(2 * entity->time) + 1));
			//movement.y = - ((-(pow(sin(entity->time), 3) + 3 * sin(entity->time) * pow(cos(entity->time), 2) + 1))) /2;


			entity->time += 0.01;
			if (entity->m_enabled != false && entity->m_sprite.getPosition().x <= 0) {
				entity->m_enabled = false;
				BossMeteorNumber--;
			}
		}


		entity->m_sprite.move(movement);
	}

	if (mBackground.getPosition().x > -550.0) {
		mBackground.move(-bgMove, 0);
	}
}


void Game::meteorShower() {
	std::srand(time(0));
	for (int i = 0; i < 5; i++)
	{
		std::shared_ptr<Entity> em = std::make_shared<Entity>();
		em->m_sprite.setTexture(mTMeteor);
		em->m_size = mTMeteor.getSize();
		em->m_sprite.setPosition(900, (mWindow.getSize().y * (i + 1) / 7) + std::rand() % 100) ;
		em->m_sprite.setScale(0.1f, 0.1f);
		em->m_type = EntityType::enemyMasterWeapon;
		em->damage = 20;
		em->life = 40;
		em->time = 0;
		EntityManager::m_Entities.push_back(em);
	}
	BossMeteorNumber = 5;
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

	if (mGameOver) {
		displayGameOver();

		if (sGameover.getStatus() != sf::SoundSource::Playing) {
			sGameover.play();
			sBackground.stop();
		}
	}

	if (mVictory) {
		displayVictory();
		if (sVictory.getStatus() != sf::SoundSource::Playing) {
			sVictory.play();
			sBackground.stop();
		}
	}

	displayLifeText();

	mWindow.display();
}

void Game::displayGameOver() {
	mWindow.draw(mGameOverMessage);
	sBackground.stop();
}

void Game::displayVictory() {
	mWindow.draw(mVictoryMessage);
	sBackground.stop();
}

void Game::displayLifeText() {
	if (mGameOver) {
		lifeDisplay.setString("0 %");
		mWindow.draw(lifeDisplay);
	} else {
		int life = EntityManager::GetPlayer()->life;
		std::string life_str = "";

		if (life <= 0) {
			life_str += "0";
		} else {
			life_str += std::to_string(life * 2);
		}

		life_str += " %";
		lifeDisplay.setString(life_str);
		mWindow.draw(lifeDisplay);
	}
	
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
		sw->damage = 10;
		sw->life = 1;
		EntityManager::m_Entities.push_back(sw);
		_IsPlayerWeaponFired = true;
		sLaser.play();
	}

	if (key == sf::Keyboard::Escape) {
		if (mGameOver || mVictory) {
			mWindow.close();
		}
	}
}

void Game::handleCollisions()
{
	std::shared_ptr<Entity> player = EntityManager::GetPlayer();
	sf::FloatRect boundPlayer = player->m_sprite.getGlobalBounds();

	for (std::shared_ptr<Entity> entity : EntityManager::m_Entities) {
		if (entity->m_enabled == false)
		{
			continue;
		}
		sf::FloatRect boundWeapon;
		boundWeapon = entity->m_sprite.getGlobalBounds();

		if (entity->m_type == EntityType::weapon)
		{
			if (entity->m_sprite.getPosition().x > mWindow.getSize().x)
			{
				entity->m_enabled = false;
			}
			for (std::shared_ptr<Entity> enemy : EntityManager::m_Entities)
			{
				if (enemy->m_enabled == false)
				{
					continue;
				}
				else if (enemy->m_type == EntityType::enemy || enemy->m_type == EntityType::enemyMaster)
				{
					sf::FloatRect boundEnemy;
					boundEnemy = enemy->m_sprite.getGlobalBounds();
					if (boundEnemy.intersects(boundWeapon) == true)
					{
						enemy->life = enemy->life - player->damage;
						if (enemy->life <= 0)
						{
							enemy->m_enabled = false;
							if (enemy->m_type == EntityType::enemyMaster) {
								mVictory = true;
								sVictory.play();
								sBackground.stop();
							}
							sExplosion.play();
						}
						entity->m_enabled = false;
						break;
					}
				}

				else if (enemy->m_type == EntityType::enemyWeapon)
				{
					sf::FloatRect boundMasterWeapon;
					boundMasterWeapon = enemy->m_sprite.getGlobalBounds();
					if (boundMasterWeapon.intersects(boundWeapon) == true)
					{
						enemy->life -= player->damage;
						if (enemy->life <= 0)
						{
							entity->m_enabled = false;
							enemy->m_enabled = false;
							sExplosion.play();
						}
						break;
					}
				}
				else if (enemy->m_type == EntityType::enemyMasterWeapon)
				{
					sf::FloatRect boundMasterWeapon;
					boundMasterWeapon = enemy->m_sprite.getGlobalBounds();
					if (boundMasterWeapon.intersects(boundWeapon) == true)
					{
						enemy->life -= player->damage;
						if (enemy->life <= 0)
						{
							enemy->m_enabled = false;+
							BossMeteorNumber--;
							sExplosion.play();
						}
						break;
					}
				}
			}
		}

		if (entity->m_type == EntityType::enemyWeapon || // Enemy hit player
			entity->m_type == EntityType::enemy ||
			entity->m_type == EntityType::enemyMasterWeapon ||
			entity->m_type == EntityType::enemyMaster)
		{
			if (entity->m_type == EntityType::enemy)
			{
				if (entity->m_sprite.getPosition().x < 0 - (entity->m_sprite.getTexture()->getSize().x * entity->m_sprite.getScale().x))
				{
					player->life = player->life - entity->damage;
					entity->m_enabled = false;
					break;
				}
			}
			if (boundWeapon.intersects(boundPlayer) == true)
			{
				player->life = player->life - entity->damage;
				if (entity->m_type != EntityType::enemyMaster) // Can't OS the boss
					entity->m_enabled = false;
				if (entity->m_type == EntityType::enemyMasterWeapon) {
					BossMeteorNumber--;
				}
				break;
			}
			if (player->life <= 0) {
				mGameOver = true;
				player->m_enabled = false;
			}
		}
	}

	if (!mVictory && EntityManager::GetEnemyMaster()->m_sprite.getPosition().x <= 900) {
		if (BossMeteorNumber == 0) {
			meteorShower();
		}
	}
}
