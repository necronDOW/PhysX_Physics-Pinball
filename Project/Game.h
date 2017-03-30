#ifndef game_h
#define game_h

#include "PxPhysicsAPI.h"
#include "Extras/HUD.h"

class Game
{
	/// SINGLETON
	/// Private singleton members, this hides the constructor, destructor and copy constructor.
	private:
			static Game* _instance;
			Game() { }
			Game(const Game* o) { }
			~Game() { }
	/// SINGLETON

	/// SINGLETON
	/// Public accessor for getting a singleton instance, this creates an instance if none already exists and
	/// stores it as _instance, before returning this.
	public:
		static Game& Instance()
		{
			if (_instance == nullptr)
				_instance = new Game();
			return *_instance;
		}
	/// SINGLETON

	private:
		int _multiplier = 1;
		int _streak = 0;
		int _score = 0;
		int _lives = 5;
		bool _gameOver = false;
		bool _resetNextUpdate = false;
		physx::PxRigidActor* _player;
		physx::PxTransform _initialPlayerPosition;
		VisualDebugger::HUD* _hud;

		void CheckState();

	public:
		int score();
		void score(int modifier);
		void lives(int modifier);
		bool gameover();
		void player(physx::PxActor* player);
		void hud(VisualDebugger::HUD* hud);
		void Reset();
		void ResetPlayer();
		void Update();
};

#endif