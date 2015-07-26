#include "eval/RandomEvaluator.h"

#include <cassert>
#include <iostream>
#include <memory>

#include "common/GameAction.h"
#include "common/ProgramOptions.h"

using namespace std;

RandomEvaluator::RandomEvaluator(std::shared_ptr<ProgramOptions> programOptions)
	: programOptions(programOptions), randomEngine(programOptions->randomSeed) {
}

GameAction RandomEvaluator::bestAction(GameState gameState) {
	vector<GameAction> enabledActions;
	if (gameState.isActionEnabled(LEFT)) enabledActions.push_back(LEFT);
	if (gameState.isActionEnabled(RIGHT)) enabledActions.push_back(RIGHT);
	if (gameState.isActionEnabled(UP)) enabledActions.push_back(UP);
	if (gameState.isActionEnabled(DOWN)) enabledActions.push_back(DOWN);

	assert(!enabledActions.empty());

	uniform_int_distribution<> uniformDistribution(0, enabledActions.size()-1);
	return enabledActions[uniformDistribution(randomEngine)];
}
