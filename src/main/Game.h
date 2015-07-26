#ifndef SRC_COMMON_GAME_H_
#define SRC_COMMON_GAME_H_

#include <memory>
#include <random>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"

#include "eval/IEvaluator.h"

class Game {
public:
	explicit Game(std::shared_ptr<ProgramOptions> programOptions);
	int play();

private:
	void initialize();
	bool isTerminalState();
	GameAction bestAction();
	int makeMove(GameAction);
	int computeAfterstate(GameAction);
	void addRandomTile();

	std::shared_ptr<ProgramOptions> programOptions;

	int score;
	GameState gameState;

	std::default_random_engine randomEngine;

	std::shared_ptr<IEvaluator> evaluator;
};

#endif  // SRC_COMMON_GAME_H_
