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

	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
	void handleCollisions();


private:
	static const float		PlayerSpeed;
	static const float		PlayerMissileSpeed;
	static const float		BackgroundSpeed;
	static const float		EnemySpeed;
	static const float		EnemyMissilesSpeed;
	static int		EnemyMissilesNumber;

	sf::RenderWindow		mWindow;
	sf::Texture	mTexture;
	sf::Sprite	mPlayer;
	sf::Texture mTMissile;
	sf::Sprite mMissile;
	sf::Texture mTBackground;
	sf::Sprite mBackground;

	sf::Texture			mTEnemy;
	sf::Sprite			mEnemy[8];

	sf::Texture			mTEnemyMissile;
	sf::Sprite			mEnemyMissile;

	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingRight;
	bool mIsMovingLeft;

	bool _IsPlayerWeaponFired = false;


	sf::Texture	_TextureWeapon;
};

 