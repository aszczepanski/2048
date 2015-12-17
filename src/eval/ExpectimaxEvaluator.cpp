#include "eval/ExpectimaxEvaluator.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <list>
#include <thread>
#include <future>
#include <mutex>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/GameStats.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

using namespace std;

const TupleValueType ExpectimaxEvaluator::MIN_PROBABILITY_THRESHOLD = 0.0001;
const TupleValueType ExpectimaxEvaluator::EPS = 1e-2;
const TupleValueType ExpectimaxEvaluator::MIN_TUPLE_VALUE = -100000.0;

ExpectimaxEvaluator::ExpectimaxEvaluator(shared_ptr<ProgramOptions> programOptions,
			shared_ptr<TuplesDescriptor> tuplesDescriptor)
	: programOptions(programOptions), tuplesDescriptor(tuplesDescriptor) {
}

void ExpectimaxEvaluator::reset() {
	transpositionTable.clear();

#ifdef EVALUATOR_GATHER_STATS
	depths.fill(UINT64_C(0));
	cacheHits = UINT64_C(0);
#endif
}

GameAction ExpectimaxEvaluator::bestAction(GameState gameState) {
	transpositionTable.clear();

	GameAction action = bestActionInternal(gameState);

#ifdef EVALUATOR_GATHER_STATS
	if (programOptions->verbose) {
		copy(depths.begin(), depths.end(), ostream_iterator<unsigned long long>(cout, " "));
		cout << endl;
	}
#endif

	return action;
}

GameAction ExpectimaxEvaluator::bestActionInternal(GameState gameState) {
	GameAction action = NO_ACTION;

	maxDepth = programOptions->maxDepth;

#ifdef EVALUATOR_GATHER_STATS
	depths[maxDepth]++;
#endif

	if (programOptions->evalMultithreading) {
		action = visitTopLevelActionNodeMultiThreading(gameState);
	} else {
		action = visitTopLevelActionNodeSingleThreading(gameState);
	}

	return action;
}

GameAction ExpectimaxEvaluator::visitTopLevelActionNodeSingleThreading(GameState gameState) {
	TupleValueType maxValue = MIN_TUPLE_VALUE;
	GameAction maxAction = NO_ACTION;

	for (GameAction action : gameActions) {
		GameState newState = gameState;
		newState.computeAfterstate(action);

		if (gameState == newState) continue;

		if (maxAction == NO_ACTION) maxAction = action;

		TupleValueType stateValue = visitRandomNode(maxDepth, 1.0 /* 1/4 */, newState) + EPS;

		if (stateValue > maxValue) {
			maxValue = stateValue;
			maxAction = action;
		}
	}

	assert(maxAction != NO_ACTION);

	return maxAction;
}

GameAction ExpectimaxEvaluator::visitTopLevelActionNodeMultiThreading(GameState gameState) {
	TupleValueType maxValue = MIN_TUPLE_VALUE;
	GameAction maxAction = NO_ACTION;

	array<thread, 4> threads;
	array<TupleValueType, 4> stateValues;
	array<GameAction, 4> allowedActions;

	size_t pos = 0;

	for (GameAction action : gameActions) {
		GameState newState = gameState;
		newState.computeAfterstate(action);

		if (gameState == newState) continue;

		if (maxAction == NO_ACTION) maxAction = action;

		auto fun = [=] (TupleValueType& stateValue) {
			stateValue = visitRandomNode(maxDepth, 1.0, newState) + EPS;
		};
		threads[pos] = thread(fun, ref(stateValues[pos]));
		allowedActions[pos++] = action;
	}
	for (size_t i=0; i<pos; i++) {
		threads[i].join();
		if (stateValues[i] > maxValue) {
			maxValue = stateValues[i];
			maxAction = allowedActions[i];
		}
	}

	assert(maxAction != NO_ACTION);

	return maxAction;
}

TupleValueType ExpectimaxEvaluator::visitActionNode(uint8_t depth, TupleValueType probability, GameState gameState) {
	TupleValueType maxValue = MIN_TUPLE_VALUE;

	for (GameAction action : gameActions) {
		GameState newState = gameState;
		newState.computeAfterstate(action);

		if (gameState == newState) continue;

		TupleValueType stateValue = visitRandomNode(depth-1, probability /* *1/4 */, newState);
		maxValue = max(maxValue, stateValue);
	}

	return maxValue;
}

TupleValueType ExpectimaxEvaluator::visitRandomNode(uint8_t depth, TupleValueType probability, GameState gameState) {
	if (isTimeLimitExceeded()) {
		return MIN_TUPLE_VALUE;
	}

	if (depth <= 1 || probability < MIN_PROBABILITY_THRESHOLD) {
		return tuplesDescriptor->evaluateState(gameState);
	}

	{
		lock_guard<mutex> lock(transpositionTableMutex);
		unordered_map<uint64_t, pair<uint8_t, TupleValueType> >::const_iterator it = transpositionTable.find(gameState);
		if (it != transpositionTable.end()) {
			pair<uint8_t, TupleValueType> entry = it->second;
			if (entry.first >= depth) {
#ifdef EVALUATOR_GATHER_STATS
				cacheHits++;
#endif
				return entry.second;
			}
		}
	}

	TupleValueType numOpen = gameState.countEmpty();

	probability /= numOpen;  // numOpen is always greater than 0
	TupleValueType tile2probability = probability*GameState::TILE_2_PROBABILITY;
	TupleValueType tile4probability = probability*GameState::TILE_4_PROBABILITY;

	TupleValueType alpha = 0.0;

	for (uint8_t i=UINT8_C(0); i<UINT8_C(16); ++i) {
		if (gameState.getTileValue(i) == UINT8_C(0)) {
			// cout << "visiting " << i << endl;
			gameState.setTileValue(i, UINT8_C(1));
			alpha += GameState::TILE_2_PROBABILITY*visitActionNode(depth, tile2probability, gameState);
			gameState.setTileValue(i, UINT8_C(2));
			alpha += GameState::TILE_4_PROBABILITY*visitActionNode(depth, tile4probability, gameState);
			gameState.clearTileValue(i);
		}
	}

	TupleValueType res = alpha / numOpen;  // numOpen is always greater than 0

	{
		lock_guard<mutex> lock(transpositionTableMutex);
		unordered_map<uint64_t, pair<uint8_t, TupleValueType> >::iterator it = transpositionTable.find(gameState);
		if (it != transpositionTable.end()) {
			pair<uint8_t, TupleValueType> entry = it->second;
			if (entry.first < depth) {
				it->second = make_pair(depth, res);
			}
		} else {
			pair<uint8_t, TupleValueType> entry = make_pair(depth, res);
			transpositionTable[gameState] = entry;
		}
	}

	return res;
}

