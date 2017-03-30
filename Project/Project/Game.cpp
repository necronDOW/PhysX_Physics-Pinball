#include "Game.h"

Game* Game::_instance = nullptr;

void Game::CheckState()
{
	// If the player is out of lives, set gameover to true.
	if (_lives <= 0)
		_gameOver = true;
}

int Game::score()
{
	return _score;
}

void Game::score(int modifier)
{
	// Add modifier * multiplier to the score and increment the multiplier if the score is a factor of 5.
	_score += modifier * _multiplier;
	_hud->EditLine(VisualDebugger::SCORE, 3, _score);

	if (++_streak % 5 == 0)
		_multiplier++;
}

void Game::lives(int modifier)
{
	// Modify the lives parameter by the given value and check necessary states.
	_lives += modifier;
	_hud->EditLine(VisualDebugger::SCORE, 1, _lives);

	// Toggles a trigger for resetting player position at the end of a physics simulation.
	ResetPlayer();
	CheckState();
}

bool Game::gameover()
{
	return _gameOver;
}

void Game::player(physx::PxActor* player)
{
	// Set the player references to that of the provided value if its type is valid (rigid actor).
	if (_player = player->isRigidActor())
	{
		physx::PxTransform playerTransform = player->isRigidDynamic()->getGlobalPose();

		if (playerTransform.isValid())
			_initialPlayerPosition = playerTransform;
	}
}

void Game::hud(VisualDebugger::HUD* hud)
{
	// Attach a given hud to the game manager and update the display to show lives and score.
	_hud = hud;

	_hud->EditLine(VisualDebugger::SCORE, 1, _lives);
	_hud->EditLine(VisualDebugger::SCORE, 3, _score);
}

void Game::Reset()
{
	// Reset all game state variables.
	_multiplier = 1;
	_streak = 0;
	_score = 0;
	_lives = 5;
	_gameOver = false;

	// Update the game HUD to show the new reset variables.
	_hud->EditLine(VisualDebugger::SCORE, 1, _lives);
	_hud->EditLine(VisualDebugger::SCORE, 3, _score);

	// Reset the player position at the end of the current simulation step.
	ResetPlayer();
}

void Game::ResetPlayer()
{
	_resetNextUpdate = true;
}

void Game::Update()
{
	// Check if game over, and if it is... Reset the game.
	if (_gameOver)
		Reset();

	// This will only trigger when the player has been scheduled for a position reset, and will only be called on PostUpdate
	// within the PhysicsEngine class.
	if (_resetNextUpdate && _initialPlayerPosition.isValid())
	{
		_player->setGlobalPose(_initialPlayerPosition);
		_player->isRigidDynamic()->setLinearVelocity(physx::PxVec3(0));

		_resetNextUpdate = false;
	}
}