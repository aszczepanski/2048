#include "main/Game.h"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <memory>
#include <iostream>
#include <random>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/GameStats.h"
#include "common/ProgramOptions.h"

#include "eval/ExpectimaxEvaluator.h"
#include "eval/IterativeExpectimaxEvaluator.h"
#include "eval/RandomEvaluator.h"

#include "io/ConsoleBoardDrawer.h"

using namespace std;

Game::Game(shared_ptr<ProgramOptions> programOptions,
		shared_ptr<TuplesDescriptor> tuplesDescriptor, unsigned seed1, unsigned seed2)
	: programOptions(programOptions),
		tuplesDescriptor(tuplesDescriptor),
		positionRandomEngine(seed1),
		valueRandomEngine(seed2)
{
	// evaluator = make_shared<RandomEvaluator>(programOptions);
	if (programOptions->maxRoundTime > 0) {
		evaluator = make_shared<IterativeExpectimaxEvaluator>(programOptions, tuplesDescriptor);
	} else {
		evaluator = make_shared<ExpectimaxEvaluator>(programOptions, tuplesDescriptor);
	}
}

unique_ptr<GameStats> Game::play() {
	evaluator->reset();

	ConsoleBoardDrawer drawer;

	chrono::time_point<chrono::system_clock> startTimePoint = chrono::system_clock::now();

	moveno = scorePenalty = 0;
	isStageOverflow = false;
	scorePenalty += 2*gameState.initialize(positionRandomEngine, valueRandomEngine);

	if (programOptions->verbose) {
		drawer.draw(gameState);
		cout << "move: " << moveno << " score: " << calculateScore() << endl;
	}

	while (!isTerminalState()) {
		GameAction action = bestAction();
		makeMove(action);
		if (programOptions->verbose) {
			drawer.draw(gameState);
			cout << "move: " << moveno << " score: " << calculateScore() << endl;
		}
	}

	chrono::time_point<chrono::system_clock> endTimePoint = chrono::system_clock::now();
	chrono::duration<int, std::milli> gameDuration
		= chrono::duration_cast<chrono::duration<int, std::milli>>(endTimePoint - startTimePoint);

	int score = calculateScore();
	return unique_ptr<GameStats>(
		(new GameStats)
			->setScore(score)
			->setMoves(moveno)
			->setDuration(gameDuration)
			->setStageOverflow(isStageOverflow)
			->setStage(gameState.calculateStage())
	);
}

bool Game::isTerminalState() {
	return gameState.isTerminalState();
}

GameAction Game::bestAction() {
	GameAction action = evaluator->bestAction(gameState);

	assert(action != NO_ACTION);
	return action;
}

void Game::makeMove(GameAction action) {
	computeAfterstate(action);
	addRandomTile();
	moveno++;
}

void Game::computeAfterstate(GameAction action) {
	gameState.computeAfterstateAndUpdateOverflow(action, &isStageOverflow);
}

void Game::addRandomTile() {
	scorePenalty += 2*gameState.addRandomTile(positionRandomEngine, valueRandomEngine);
}

int Game::calculateScore() {
	// score difference between single tiles 64k and 32k
	const int overflowPenalty = 524288;  // = (16-1)*(1<<16)-(15-1)*(1<<15)
	return gameState.scoreBoard()
		- scorePenalty
		+ (isStageOverflow ? overflowPenalty : 0);
}
