#include "eval/ExpectimaxEvaluator.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <stdexcept>
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

ExpectimaxEvaluator::ExpectimaxEvaluator(shared_ptr<ProgramOptions> programOptions,
			shared_ptr<TuplesDescriptor> tuplesDescriptor)
	: programOptions(programOptions), tuplesDescriptor(tuplesDescriptor) {
}

void ExpectimaxEvaluator::reset() {
	transpositionTable.clear();

	depths.fill(0ULL);
	cacheHits = 0ULL;
}

GameAction ExpectimaxEvaluator::bestAction(GameState gameState) {

	transpositionTable.clear();

	GameAction action = bestActionInternal(gameState);
/*
	// if (programOptions->verbose) {
		// cout << '\r';
		copy(depths.begin(), depths.end(), ostream_iterator<unsigned long long>(cout, " "));
		// cout << flush;
		cout << endl;
	// }
*/
	return action;
}

GameAction ExpectimaxEvaluator::bestActionInternal(GameState gameState) {
	GameAction action = NO_ACTION;

	maxDepth = programOptions->maxDepth;
	depths[maxDepth]++;

	action = visitTopLevelActionNode(gameState);

	return action;
}

GameAction ExpectimaxEvaluator::visitTopLevelActionNode(GameState gameState) {
	TupleValueType maxValue = -100000.0;
	GameAction maxAction = NO_ACTION;

	if (programOptions->threads && maxDepth >= 3) {
		list< pair<future<TupleValueType>, GameAction> > futures;

		for (GameAction action : gameActions) {
			GameState newState = gameState;
			newState.computeAfterstate(action);

			if (gameState == newState) continue;

			futures.push_back(make_pair(async(launch::async, [=]()->TupleValueType { return visitRandomNode(maxDepth, 1.0, newState); }), action));
		}
		for (auto& pr : futures) {
			TupleValueType stateValue = pr.first.get();
			if (stateValue >= maxValue) {
				maxValue = stateValue;
				maxAction = pr.second;
			}

			// cout << stateValue << " " << pr.second << endl;
		}
	} else {
		for (GameAction action : gameActions) {
			GameState newState = gameState;
			newState.computeAfterstate(action);

			if (gameState == newState) continue;

			TupleValueType stateValue = visitRandomNode(maxDepth, 1.0 /* 1/4 */, newState);

			// cout << action << " " << stateValue << endl;

			if (stateValue >= maxValue) {
				maxValue = stateValue;
				maxAction = action;
			}
		}
	}

	assert(maxAction != NO_ACTION);

	return maxAction;
}

TupleValueType ExpectimaxEvaluator::visitActionNode(int depth, TupleValueType probability, GameState gameState) {
	TupleValueType maxValue = -100000.0;

	for (GameAction action : gameActions) {
		GameState newState = gameState;
		newState.computeAfterstate(action);

		if (gameState == newState) continue;

		TupleValueType stateValue = visitRandomNode(depth-1, probability /* *1/4 */, newState);

		// cout << action << " " << stateValue << endl;

		maxValue = max(maxValue, stateValue);
	}

	return maxValue;
}

TupleValueType ExpectimaxEvaluator::visitRandomNode(int depth, TupleValueType probability, GameState gameState) {
	if (isTimeLimitExceeded()) {
		throw runtime_error("timeout");
	}

	if (depth <= 1 || probability < MIN_PROBABILITY_THRESHOLD) {
		return tuplesDescriptor->evaluateState(gameState);
	}

	{
		lock_guard<mutex> lock(transpositionTableMutex);
		const unordered_map<uint64_t, pair<int, TupleValueType> >::iterator& it = transpositionTable.find(gameState);
		if (it != transpositionTable.end()) {
			pair<int, TupleValueType> entry = it->second;
			if (entry.first >= depth) {
				cacheHits++;
				return entry.second;
			}
		}
	}

	int numOpen = gameState.countEmpty();

	probability /= numOpen;  // numOpen is always greater than 0
	TupleValueType tile2probability = probability*GameState::TILE_2_PROBABILITY;
	TupleValueType tile4probability = probability*GameState::TILE_4_PROBABILITY;

	TupleValueType alpha = 0.0;
	for (int i=0; i<16; i++) {
		if (gameState.getTileValue(i) == 0) {
			// cout << "visiting " << i << endl;
			gameState.setTileValue(i, 1);
			alpha += GameState::TILE_2_PROBABILITY*visitActionNode(depth, tile2probability, gameState);
			gameState.setTileValue(i, 2);
			probability = tile4probability;
			alpha += GameState::TILE_4_PROBABILITY*visitActionNode(depth, tile4probability, gameState);
			gameState.clearTileValue(i);
		}
	}

	TupleValueType res = alpha / numOpen;  // numOpen is always greater than 0

	{
		lock_guard<mutex> lock(transpositionTableMutex);
		const unordered_map<uint64_t, pair<int, TupleValueType> >::iterator& it = transpositionTable.find(gameState);
		if (it != transpositionTable.end()) {
			pair<int, TupleValueType> entry = it->second;
			if (entry.first < depth) {
				it->second = make_pair(depth, res);
			}
		} else {
			pair<int, TupleValueType> entry = make_pair(depth, res);
			transpositionTable[gameState] = entry;
		}
	}

	return res;
}

