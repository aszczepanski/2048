#include "eval/IterativeExpectimaxEvaluator.h"

#include <chrono>
#include <iostream>
#include <memory>

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

	isFinished = false;
	isTimeout = false;

	thread timer_thread([=] { timerFunction(programOptions->maxRoundTime); });

	unsigned depth;
	for (depth=1; depth<=programOptions->maxDepth; depth++) {
		try {
			maxDepth = depth;

			if (depth >= 3 && programOptions->threads) {
				bestAction = visitTopLevelActionNodeMultiThreading(gameState);
			} else {
				bestAction = visitTopLevelActionNodeSingleThreading(gameState);
			}
		} catch (...) {
			depths[maxDepth-1]++;
			break;
		}
	}

	if (depth > programOptions->maxDepth) {
		depths[programOptions->maxDepth]++;
	}

	isFinished = true;
	cv.notify_one();

	timer_thread.join();

	return bestAction;
}

void IterativeExpectimaxEvaluator::timerFunction(int millis) {
	unique_lock<mutex> lk(m);

	cv.wait_for(lk, chrono::milliseconds(millis), [=] { return isFinished; });

	isTimeout = true;

	lk.unlock();
}
