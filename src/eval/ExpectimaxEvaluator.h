#ifndef SRC_EVAL_EXPECTIMAX_EVALUATOR_H_
#define SRC_EVAL_EXPECTIMAX_EVALUATOR_H_

#include "eval/IEvaluator.h"

#include <array>
#include <memory>
#include <unordered_map>
#include <utility>

#include <thread>
#include <future>
#include <mutex>

#include "common/GameAction.h"
#include "common/GameState.h"
#include "common/ProgramOptions.h"
#include "common/TuplesDescriptor.h"

class ExpectimaxEvaluator : public IEvaluator {
public:
	ExpectimaxEvaluator(std::shared_ptr<ProgramOptions>, std::shared_ptr<TuplesDescriptor>);
	virtual void reset();
	virtual GameAction bestAction(GameState);

protected:
	virtual GameAction bestActionInternal(GameState);

	GameAction visitTopLevelActionNode(GameState);
	TupleValueType visitActionNode(int depth, TupleValueType probability, GameState);
	TupleValueType visitRandomNode(int depth, TupleValueType probability, GameState);

	virtual bool isTimeLimitExceeded() {
		return false;
	}

	int maxDepth;

	std::unordered_map<uint64_t, std::pair<int, TupleValueType> > transpositionTable;
	std::mutex transpositionTableMutex;

	std::shared_ptr<ProgramOptions> programOptions;
	std::shared_ptr<TuplesDescriptor> tuplesDescriptor;

	/* stats */
	std::array<unsigned long long, 20> depths;
	unsigned long long cacheHits;

	static const TupleValueType MIN_PROBABILITY_THRESHOLD;
};

#endif  // SRC_EVAL_EXPECTIMAX_EVALUATOR_H_
