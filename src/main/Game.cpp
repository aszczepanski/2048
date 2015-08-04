#include "main/Game.h"

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
#include "eval/RandomEvaluator.h"

#include "io/ConsoleBoardDrawer.h"

using namespace std;

Game::Game(shared_ptr<ProgramOptions> programOptions,
		shared_ptr<TuplesDescriptor> tuplesDescriptor)
	: programOptions(programOptions),
		tuplesDescriptor(tuplesDescriptor),
		positionRandomEngine(programOptions->randomSeed),
		valueRandomEngine(programOptions->randomSeed),
		// evaluator(make_shared<RandomEvaluator>(programOptions))
		evaluator(make_shared<ExpectimaxEvaluator>(programOptions, tuplesDescriptor))
{
}

unique_ptr<GameStats> Game::play() {
	evaluator->reset();

	ConsoleBoardDrawer drawer;

	chrono::time_point<chrono::system_clock> startTimePoint = chrono::system_clock::now();

	moveno = scorePenalty = 0;
	scorePenalty += 2*gameState.initialize(positionRandomEngine, valueRandomEngine);

	if (programOptions->verbose) {
		drawer.draw(gameState);
	}

	while (!isTerminalState()) {
		// getchar();
		GameAction action = bestAction();
		// cout << action << endl;
		makeMove(action);
		if (programOptions->verbose) {
			drawer.draw(gameState);
			cout << "move: " << moveno << " score: " << gameState.scoreBoard() - scorePenalty << endl;
		}
	}

	chrono::time_point<chrono::system_clock> endTimePoint = chrono::system_clock::now();
	chrono::duration<int, std::milli> gameDuration
		= chrono::duration_cast<chrono::duration<int, std::milli>>(endTimePoint - startTimePoint);

	int score = gameState.scoreBoard() - scorePenalty;
	return unique_ptr<GameStats>(
		(new GameStats)
			->setScore(score)
			->setMoves(moveno)
			->setDuration(gameDuration)
			->setStage(GameStats::calculateStage(gameState))
	);
}

bool Game::isTerminalState() {
	return gameState.isTerminalState();
}

GameAction Game::bestAction() {
	// TODO
	GameAction action = evaluator->bestAction(gameState);
	return action;
}

void Game::makeMove(GameAction action) {
	computeAfterstate(action);
	addRandomTile();
	moveno++;
}

void Game::computeAfterstate(GameAction action) {
	gameState.computeAfterstate(action);
}

void Game::addRandomTile() {
	scorePenalty += 2*gameState.addRandomTile(positionRandomEngine, valueRandomEngine);
}
