#ifndef SRC_EVAL_EXPECTIMAX_EVALUATOR_H_
#define SRC_EVAL_EXPECTIMAX_EVALUATOR_H_

#include "eval/IEvaluator.h"

#include <memory>
#include <unordered_map>
#include <utility>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

class ExpectimaxEvaluator : public IEvaluator {
public:
	ExpectimaxEvaluator(std::shared_ptr<ProgramOptions>, std::shared_ptr<TuplesDescriptor>);
	virtual void reset();
	virtual GameAction bestAction(GameState);

private:
	GameAction visitTopLevelActionNode(GameState);
	float visitActionNode(int depth, GameState);
	float visitRandomNode(int depth, GameState);

	float evaluateState(GameState);

	int maxDepth;

	std::unordered_map<uint64_t, std::pair<int, float> > transpositionTable;

	std::shared_ptr<ProgramOptions> programOptions;
	std::shared_ptr<TuplesDescriptor> tuplesDescriptor;
};

#endif  // SRC_EVAL_EXPECTIMAX_EVALUATOR_H_
