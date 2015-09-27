#include "eval/IterativeExpectimaxEvaluator.h"

#include <atomic>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

using namespace std;

IterativeExpectimaxEvaluator::IterativeExpectimaxEvaluator(shared_ptr<ProgramOptions> programOptions,
		shared_ptr<TuplesDescriptor> tuplesDescriptor)
	: ExpectimaxEvaluator(programOptions, tuplesDescriptor) {
}

GameAction IterativeExpectimaxEvaluator::bestActionInternal(GameState gameState) {
	GameAction bestAction = NO_ACTION;
	GameAction tmpAction;

	{
		lock_guard<mutex> lock(m);
		isFinished = false;
	}
	isTimeout = false;

	thread timer_thread([this] { timerFunction(programOptions->maxRoundTime); });

	unsigned depth;
	for (depth=1; depth<=programOptions->maxDepth; depth++) {
		maxDepth = depth;

		if (depth >= 3 && programOptions->threads) {
			tmpAction = visitTopLevelActionNodeMultiThreading(gameState);
		} else {
			tmpAction = visitTopLevelActionNodeSingleThreading(gameState);
		}

		if (!isTimeout) {
			bestAction = tmpAction;
		} else {
#ifdef EVALUATOR_GATHER_STATS
			depths[maxDepth-1]++;
#endif
			break;
		}
	}

#ifdef EVALUATOR_GATHER_STATS
	if (depth > programOptions->maxDepth) {
		depths[programOptions->maxDepth]++;
	}
#endif

	{
		lock_guard<mutex> lock(m);
		isFinished = true;
		cv.notify_one();
	}

	timer_thread.join();

	return bestAction;
}

void IterativeExpectimaxEvaluator::timerFunction(int millis) {
	unique_lock<mutex> lk(m);

	cv.wait_for(lk, chrono::milliseconds(millis), [=] { return isFinished.load(); });
	// this_thread::sleep_for(chrono::milliseconds(millis));

	isTimeout = true;

	lk.unlock();
}
