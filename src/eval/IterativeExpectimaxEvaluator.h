#ifndef SRC_EVAL_ITERATIVE_EXPECTIMAX_EVALUATOR_H_
#define SRC_EVAL_ITERATIVE_EXPECTIMAX_EVALUATOR_H_

#include "eval/ExpectimaxEvaluator.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

class IterativeExpectimaxEvaluator : public ExpectimaxEvaluator {
public:
	IterativeExpectimaxEvaluator(std::shared_ptr<ProgramOptions>, std::shared_ptr<TuplesDescriptor>);

protected:
	virtual GameAction bestActionInternal(GameState);

	virtual bool isTimeLimitExceeded() {
		return isTimeout && maxDepth > 1;
	}

private:
	void timerFunction(int millis);

	std::thread timerThread;
	std::mutex m;
	std::condition_variable cv;

	std::atomic_bool isFinished;
	std::atomic_bool isTimeout;

};

#endif  // SRC_EVAL_ITERATIVE_EXPECTIMAX_EVALUATOR_H_
