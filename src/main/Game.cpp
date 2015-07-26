#include "main/Game.h"

#include <cstdio>
#include <memory>
#include <iostream>
#include <random>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"

#include "eval/RandomEvaluator.h"

#include "io/ConsoleBoardDrawer.h"

using namespace std;

Game::Game(shared_ptr<ProgramOptions> programOptions)
	: programOptions(programOptions), randomEngine(programOptions->randomSeed),
		evaluator(make_shared<RandomEvaluator>(programOptions)) {
	cout << programOptions->strategy << endl;
}

int Game::play() {
	ConsoleBoardDrawer drawer;

	score = 0;
	gameState.initialize(randomEngine);
	drawer.draw(gameState);

	while (!isTerminalState()) {
		// getchar();
		GameAction action = bestAction();
		int reward = makeMove(action);
		score += reward;
		drawer.draw(gameState);
		cout << "score: " << score << endl;
	}

	return score;
}

bool Game::isTerminalState() {
	return gameState.isTerminalState();
}

GameAction Game::bestAction() {
	// TODO
	GameAction action = evaluator->bestAction(gameState);
	return action;
}

int Game::makeMove(GameAction action) {
	int reward = computeAfterstate(action);
	addRandomTile();
	return reward;
}

int Game::computeAfterstate(GameAction action) {
	return gameState.computeAfterstate(action);
}

void Game::addRandomTile() {
	gameState.addRandomTile(randomEngine);
}
