#ifndef SRC_COMMON_TUPLES_DESCRIPTOR_H_
#define SRC_COMMON_TUPLES_DESCRIPTOR_H_

#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <vector>

#include <iostream>

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
		return gameState.calculateStage(stageBitsOffset);
	}

	uint8_t stageBits;
	uint8_t stageBitsOffset;
};

struct ExpandedTuplesDescriptor : public TuplesDescriptor {
	virtual TupleValueType evaluateState(GameState gameState) {
		TupleValueType stateValue = 0.0;

		uint8_t stageEntry = getStageEntry(gameState);

		size_t lup;
		for (const ExpandedTuple& tuple : tuples[stageEntry]) {
			for (const std::vector<uint8_t>& lup_mm : tuple.pts) {
				lup = 0;
				for (auto it = lup_mm.begin(); it != lup_mm.end(); ++it) {
					lup <<= 4;
					lup |= gameState.getTileValue(*it);
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

		size_t lup;
		for (const CompressedTuple& tuple : tuples[stageEntry]) {
			for (const std::vector<uint8_t>& lup_mm : tuple.pts) {
				lup = 0;
				for (size_t i=0; i<lup_mm.size()-1; ++i) {
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
