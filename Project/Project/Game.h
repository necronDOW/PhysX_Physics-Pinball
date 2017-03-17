#ifndef game_h
#define game_h

class Game
{
	private:
		/// SINGLETON
		static Game* _instance;
		Game() { }
		Game(const Game* o) { }
		~Game() { }
		/// SINGLETON

		int _score = 0;
		int _lives = 5;
		bool _gameOver = false;

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
};

#endif