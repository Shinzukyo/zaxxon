#pragma once


class Game
{
public:
	Game();
	~Game() { };
	void run();

private:
	void processEvents();
	void update();
	void render();

	void InitSprites();
	void ResetSprites();

	void displayGameOver();
	void displayVictory();

	void displayLifeText();

	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
	void handleCollisions();
	void meteorShower();


private:
	static const float		PlayerSpeed;
	static const float		PlayerMissileSpeed;
	static const float		BackgroundSpeed;
	static const float		EnemySpeed;
	static const float		EnemyMissilesSpeed;
	static int		BossMeteorNumber;

	bool mGameOver;
	bool mVictory;

	sf::RenderWindow		mWindow;
	sf::Texture	mTexture;
	sf::Sprite	mPlayer;
	sf::Texture mTMissile;
	sf::Sprite mMissile;
	sf::Texture mTBackground;
	sf::Sprite mBackground;

	sf::Texture	mTBoss;
	sf::Sprite	mBoss;

	sf::Texture	mTGameOverMessage;
	sf::Sprite	mGameOverMessage;

	sf::Texture	mTVictoryMessage;
	sf::Sprite	mVictoryMessage;

	sf::Texture			mTEnemy;
	sf::Sprite			mEnemy[8];

	sf::Texture			mTEnemyMissile;
	sf::Sprite			mEnemyMissile;

	sf::Texture mTMeteor;
	sf::Sprite mMeteor;

	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingRight;
	bool mIsMovingLeft;

	bool _IsPlayerWeaponFired = false;


	sf::Texture	_TextureWeapon;

	sf::Text 			lifeDisplay;
	sf::Font			lifeFont;
};

 