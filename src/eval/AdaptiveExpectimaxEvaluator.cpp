#include "eval/AdaptiveExpectimaxEvaluator.h"

#include <algorithm>
#include <memory>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

using namespace std;

AdaptiveExpectimaxEvaluator::AdaptiveExpectimaxEvaluator(shared_ptr<ProgramOptions> programOptions,
		shared_ptr<TuplesDescriptor> tuplesDescriptor)
	: ExpectimaxEvaluator(programOptions, tuplesDescriptor) {
}

GameAction AdaptiveExpectimaxEvaluator::bestActionInternal(GameState gameState) {
	GameAction action = NO_ACTION;

	maxDepth = max(4, gameState.countDistinctTiles()-5);
	depths[maxDepth]++;

	action = visitTopLevelActionNode(gameState);

	return action;
}
