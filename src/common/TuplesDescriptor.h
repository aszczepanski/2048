#ifndef SRC_COMMON_TUPLES_DESCRIPTOR_H_
#define SRC_COMMON_TUPLES_DESCRIPTOR_H_

#include <array>
#include <cassert>
#include <cstdint>
#include <memory>
#include <vector>

#include <common/GameState.h>

typedef double TupleValueType;

struct Tuple {
	std::vector< std::vector<uint8_t> > pts;  // of size m*n
};

struct ExpandedTuple : public Tuple {
	std::vector<TupleValueType> lut;  // of size 16^n
};

struct CompressedTuple : public Tuple {
	std::vector< std::shared_ptr< std::array<TupleValueType, 16> > > lutPtrs;  // of size 16^(n-1) pointers to arrays
};

struct TuplesDescriptor {
	virtual TupleValueType evaluateState(GameState) = 0;

	uint8_t getStageEntry(GameState gameState) {
		uint16_t stage = gameState.calculateStage();

		uint8_t stageEntry = 0;
		for (uint8_t i=0; i<stageBits; i++) {
			stageEntry <<= 1;
			stageEntry |= (stage & (1<<(15-i))) ? 1 : 0;
		}

		return stageEntry;
	}

	// std::vector<uint8_t> T;  // of size stages
	uint8_t stageBits;
};

struct ExpandedTuplesDescriptor : public TuplesDescriptor {
	virtual TupleValueType evaluateState(GameState gameState) {
		TupleValueType stateValue = 0.0;

		uint8_t stageEntry = getStageEntry(gameState);

		for (const ExpandedTuple& tuple : tuples[stageEntry]) {
			for (const std::vector<uint8_t>& lup_mm : tuple.pts) {
				size_t lup = 0;
				for (size_t i=0; i<lup_mm.size(); i++) {
					lup <<= 4;
					lup |= gameState.getTileValue(lup_mm[i]);
				}
				stateValue += tuple.lut[lup];
			}
		}

		return stateValue;
	}

	std::vector< std::vector<ExpandedTuple> > tuples;  // of size stages
};

struct CompressedTuplesDescriptor : public TuplesDescriptor {
	virtual TupleValueType evaluateState(GameState gameState) {
		TupleValueType stateValue = 0.0;

		uint8_t stageEntry = getStageEntry(gameState);

		for (const CompressedTuple& tuple : tuples[stageEntry]) {
			for (const std::vector<uint8_t>& lup_mm : tuple.pts) {
				size_t lup = 0;
				for (size_t i=0; i<lup_mm.size()-1; i++) {
					lup <<= 4;
					lup |= gameState.getTileValue(lup_mm[i]);
				}
				stateValue += tuple.lutPtrs[lup]->at(gameState.getTileValue(lup_mm.back()));
			}
		}

		return stateValue;

	}

	std::vector< std::vector<CompressedTuple> > tuples;  // of size stages
};

#endif  // SRC_COMMON_TUPLES_DESCRIPTOR_H_
