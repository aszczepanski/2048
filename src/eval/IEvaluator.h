#ifndef SRC_EVAL_IEVALUATOR_H_
#define SRC_EVAL_IEVALUATOR_H_

#include "common/GameAction.h"
#include "common/GameState.h"

class IEvaluator {
public:
	virtual GameAction bestAction(GameState) = 0;
};

#endif  // SRC_EVAL_IEVALUATOR_H_