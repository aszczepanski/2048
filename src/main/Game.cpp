#include "main/Game.h"

#include <cstdio>
#include <memory>
#include <iostream>
#include <random>

#include "common/GameAction.h"
#include "common/GameState.h"
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

int Game::play() {
	evaluator->reset();

	ConsoleBoardDrawer drawer;

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
			cout << "move: " << ++moveno << " score: " << gameState.scoreBoard() - scorePenalty << endl;
		}
	}

	return gameState.scoreBoard() - scorePenalty;
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
}

void Game::computeAfterstate(GameAction action) {
	gameState.computeAfterstate(action);
}

void Game::addRandomTile() {
	scorePenalty += 2*gameState.addRandomTile(positionRandomEngine, valueRandomEngine);
}
