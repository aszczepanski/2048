#ifndef SRC_EVAL_RANDOM_EVALUATOR_H_
#define SRC_EVAL_RANDOM_EVALUATOR_H_

#include "eval/IEvaluator.h"

#include <memory>
#include <random>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"

class RandomEvaluator : public IEvaluator {
public:
	RandomEvaluator(std::shared_ptr<ProgramOptions>);
	virtual void reset() {}
	virtual GameAction bestAction(GameState);

private:
	std::shared_ptr<ProgramOptions> programOptions;

	std::default_random_engine randomEngine;
};

#endif  // SRC_EVAL_RANDOM_EVALUATOR_H_
