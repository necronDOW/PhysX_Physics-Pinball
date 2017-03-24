#include "Game.h"

Game* Game::_instance = nullptr;

void Game::CheckState()
{
	if (_lives <= 0)
		_gameOver = true;
}

int Game::score()
{
	return _score;
}

void Game::score(int modifier)
{
	_score += modifier * _multiplier;
	_hud->EditLine(VisualDebugger::SCORE, 3, _score);

	if (++_streak % 5 == 0)
		_multiplier++;
}

void Game::lives(int modifier)
{
	_lives += modifier;
	_hud->EditLine(VisualDebugger::SCORE, 1, _lives);

	ResetPlayer();
	CheckState();
}

bool Game::gameover()
{
	return _gameOver;
}

void Game::player(physx::PxActor* player)
{
	if (_player = player->isRigidActor())
	{
		physx::PxTransform playerTransform = player->isRigidDynamic()->getGlobalPose();

		if (playerTransform.isValid())
			_initialPlayerPosition = playerTransform;
	}
}

void Game::hud(VisualDebugger::HUD* hud)
{
	_hud = hud;

	_hud->EditLine(VisualDebugger::SCORE, 1, _lives);
	_hud->EditLine(VisualDebugger::SCORE, 3, _score);
}

void Game::Reset()
{
	_multiplier = 1;
	_streak = 0;
	_score = 0;
	_lives = 5;
	_gameOver = false;

	_hud->EditLine(VisualDebugger::SCORE, 1, _lives);
	_hud->EditLine(VisualDebugger::SCORE, 3, _score);

	ResetPlayer();
}

void Game::ResetPlayer()
{
	_resetNextUpdate = true;
}

void Game::Update()
{
	if (_gameOver)
		Reset();

	if (_resetNextUpdate && _initialPlayerPosition.isValid())
	{
		_player->setGlobalPose(_initialPlayerPosition);
		_player->isRigidDynamic()->setLinearVelocity(physx::PxVec3(0));

		_resetNextUpdate = false;
	}
}