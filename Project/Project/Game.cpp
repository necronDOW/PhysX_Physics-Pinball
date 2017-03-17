#include "Game.h"

Game* Game::_instance = nullptr;

int Game::score()
{
	return _score;
}

void Game::score(int modifier)
{
	_score += modifier;
}

void Game::lives(int modifier)
{
	_lives += modifier;
	CheckState();
}

bool Game::gameover()
{
	return _gameOver;
}

void Game::CheckState()
{
	if (_lives <= 0)
		_gameOver = true;
}