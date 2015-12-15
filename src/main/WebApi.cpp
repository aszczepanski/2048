#include "main/WebApi.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "common/GameState.h"
#include "common/GameStats.h"

#include "common/TuplesDescriptor.h"
#include "io/IInputFileReader.h"
#include "io/BinaryInputFileReader.h"
#include "io/TextInputFileReader.h"

#include "eval/IEvaluator.h"
#include "eval/ExpectimaxEvaluator.h"
#include "eval/IterativeExpectimaxEvaluator.h"
#include "eval/RandomEvaluator.h"

using namespace std;

shared_ptr<ProgramOptions> programOptions;
shared_ptr<TuplesDescriptor> tuplesDescriptor;
shared_ptr<IEvaluator> evaluator;

void setStrategy(const char* strategy) {
	if (!programOptions) {
		programOptions = make_shared<ProgramOptions>();
	}
	programOptions->strategy = string(strategy);
}

void setUnzip(bool unzip) {
	if (!programOptions) {
		programOptions = make_shared<ProgramOptions>();
	}
	programOptions->unzip = unzip;
}

void setMaxTime(int time) {
	if (!programOptions) {
		programOptions = make_shared<ProgramOptions>();
	}
	programOptions->maxRoundTime = time;
}

void setMaxDepth(int depth) {
	if (!programOptions) {
		programOptions = make_shared<ProgramOptions>();
	}
	programOptions->maxDepth = depth;
}

void setThreads(bool threads) {
	if (!programOptions) {
		programOptions = make_shared<ProgramOptions>();
	}
	programOptions->threads = threads;
}

bool loadTuplesDescriptor() {
	shared_ptr<IInputFileReader> inputFileReader;
	string fn = programOptions->strategy;
	if (fn.substr(fn.find_last_of(".") + 1) == "txt") {
		inputFileReader = make_shared<TextInputFileReader>();
	} else {
		inputFileReader = make_shared<BinaryInputFileReader>();
	}

	tuplesDescriptor = inputFileReader->read(
		programOptions->strategy, programOptions->unzip);

	return tuplesDescriptor != nullptr;
}

void initializeTables() {
	GameState::initializeTables();
}

void initializeEvaluator() {
	// evaluator = make_shared<RandomEvaluator>(programOptions);
	// TODO
	if (programOptions->maxRoundTime > 0) {
		evaluator = make_shared<IterativeExpectimaxEvaluator>(programOptions, tuplesDescriptor);
	} else {
		evaluator = make_shared<ExpectimaxEvaluator>(programOptions, tuplesDescriptor);
	}
	evaluator->reset();
}

int bestAction(GameState gameState) {
	GameAction action = evaluator->bestAction(gameState);
	switch (action) {
	case UP: return ACTION_CODE_UP;
	case DOWN: return ACTION_CODE_DOWN;
	case LEFT: return ACTION_CODE_LEFT;
	default: return ACTION_CODE_RIGHT;
	}
}
