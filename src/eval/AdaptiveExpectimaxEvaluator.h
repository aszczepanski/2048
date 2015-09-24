#ifndef SRC_EVAL_ADAPTIVE_EXPECTIMAX_EVALUATOR_H_
#define SRC_EVAL_ADAPTIVE_EXPECTIMAX_EVALUATOR_H_

#include "eval/ExpectimaxEvaluator.h"

#include <memory>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

class AdaptiveExpectimaxEvaluator : public ExpectimaxEvaluator {
public:
	AdaptiveExpectimaxEvaluator(std::shared_ptr<ProgramOptions>, std::shared_ptr<TuplesDescriptor>);

protected:
	virtual GameAction bestActionInternal(GameState);
};

#endif  // SRC_EVAL_ADAPTIVE_EXPECTIMAX_EVALUATOR_H_
