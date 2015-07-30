#include "eval/ExpectimaxEvaluator.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

using namespace std;

ExpectimaxEvaluator::ExpectimaxEvaluator(shared_ptr<ProgramOptions> programOptions,
			shared_ptr<TuplesDescriptor> tuplesDescriptor)
	: programOptions(programOptions), tuplesDescriptor(tuplesDescriptor) {
}

void ExpectimaxEvaluator::reset() {
	transpositionTable.clear();
}

GameAction ExpectimaxEvaluator::bestAction(GameState gameState) {

	maxDepth = programOptions->maxDepth;

	return visitTopLevelActionNode(gameState);
}

GameAction ExpectimaxEvaluator::visitTopLevelActionNode(GameState gameState) {
	float maxValue = -200000.f;
	GameAction maxAction = NO_ACTION;

	for (GameAction action : gameActions) {
		GameState newState = gameState;
		newState.computeAfterstate(action);

		if (gameState == newState) continue;

		float stateValue = visitRandomNode(1, newState);

		// cout << action << " " << stateValue << endl;

		if (stateValue >= maxValue) {
			maxValue = stateValue;
			maxAction = action;
		}
	}

	assert(maxAction != NO_ACTION);

	return maxAction;
	
}

float ExpectimaxEvaluator::visitActionNode(int depth, GameState gameState) {
	float maxValue = -200000.f;

	for (GameAction action : gameActions) {
		GameState newState = gameState;
		newState.computeAfterstate(action);

		if (gameState == newState) continue;

		float stateValue = visitRandomNode(depth+1, newState);

		// cout << action << " " << stateValue << endl;

		maxValue = max(maxValue, stateValue);
	}

	return maxValue;
}

float ExpectimaxEvaluator::visitRandomNode(int depth, GameState gameState) {
	if (depth >= maxDepth) return evaluateState(gameState);

	const unordered_map<uint64_t, pair<int, float> >::iterator& it = transpositionTable.find(gameState);
	if (it != transpositionTable.end()) {
		pair<int, float> entry = it->second;
		if (entry.first <= depth) {
			return entry.second;
		}
	}

	float alpha = 0.f;
	int nonZeros = 0;
	for (int i=0; i<16; i++) {
		if (gameState.getTileValue(i) == 0) {
			// cout << "visiting " << i << endl;
			nonZeros++;
			gameState.setTileValue(i, 1);
			alpha += GameState::TILE_2_PROBABILITY*visitActionNode(depth, gameState);
			gameState.setTileValue(i, 2);
			alpha += GameState::TILE_4_PROBABILITY*visitActionNode(depth, gameState);
			gameState.clearTileValue(i);
		}
	}

	float res = (nonZeros > 0) ? (alpha/(float)nonZeros) : alpha;

	pair<int, float> entry = make_pair<int, float>(depth, res);
	transpositionTable[gameState] = entry;

	return res;
}

float ExpectimaxEvaluator::evaluateState(GameState gameState) {
	float stateValue = 0.f;

	for (int t=0; t<tuplesDescriptor->T; t++) {  // TODO: for each
		Tuple* tuple = &tuplesDescriptor->tuples[t];
		for (int mm=0; mm<tuple->m; mm++) {
			int lup = 0;
			for (int nn=0; nn<tuple->n; nn++) {
				lup <<= 4;
				lup |= gameState.getTileValue(tuple->pts[mm][nn]);
			}
			stateValue += tuple->lut[lup];
		}
	}

	return stateValue;
}
