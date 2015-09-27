#ifndef SRC_EVAL_IEVALUATOR_H_
#define SRC_EVAL_IEVALUATOR_H_

// #define EVALUATOR_GATHER_STATS

#include "common/GameAction.h"
#include "common/GameState.h"

class IEvaluator {
public:
	virtual void reset() = 0;
	virtual GameAction bestAction(GameState) = 0;
};

#endif  // SRC_EVAL_IEVALUATOR_H_
