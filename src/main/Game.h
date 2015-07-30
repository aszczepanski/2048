#ifndef SRC_COMMON_GAME_H_
#define SRC_COMMON_GAME_H_

#include <memory>
#include <random>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

#include "eval/IEvaluator.h"

class Game {
public:
	explicit Game(std::shared_ptr<ProgramOptions>, std::shared_ptr<TuplesDescriptor>);
	int play();

private:
	void initialize();
	bool isTerminalState();
	GameAction bestAction();
	void makeMove(GameAction);
	void computeAfterstate(GameAction);
	void addRandomTile();

	std::shared_ptr<ProgramOptions> programOptions;
	std::shared_ptr<TuplesDescriptor> tuplesDescriptor;

	int moveno;
	int scorePenalty;
	GameState gameState;

	std::default_random_engine positionRandomEngine;
	std::default_random_engine valueRandomEngine;

	std::shared_ptr<IEvaluator> evaluator;
};

#endif  // SRC_COMMON_GAME_H_
