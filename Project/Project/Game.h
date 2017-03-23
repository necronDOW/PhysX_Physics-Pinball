#ifndef game_h
#define game_h

#include "PxPhysicsAPI.h"

class Game
{
	private:
		/// SINGLETON
		static Game* _instance;
		Game() { }
		Game(const Game* o) { }
		~Game() { }
		/// SINGLETON

		int _multiplier = 1;
		int _streak = 0;
		int _score = 0;
		int _lives = 5;
		bool _gameOver = false;
		bool _resetNextUpdate = false;
		physx::PxRigidActor* _player;
		physx::PxTransform _initialPlayerPosition;

		void CheckState();

	public:
		/// SINGLETON
		static Game& Instance()
		{
			if (_instance == nullptr)
				_instance = new Game();
			return *_instance;
		}
		/// SINGLETON

		int score();
		void score(int modifier);
		void lives(int modifier);
		bool gameover();
		void player(physx::PxActor* player);
		void Reset();
		void ResetPlayer();
		void Update();
};

#endif