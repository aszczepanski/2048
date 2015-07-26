#ifndef SRC_COMMON_GAME_STATE_H_
#define SRC_COMMON_GAME_STATE_H_

#include <cstddef>
#include <cstdint>
#include <random>

#include "common/GameAction.h"

class GameState {
public:
	void initialize(std::default_random_engine&);
	bool isTerminalState();
	bool isActionEnabled(GameAction);
	int computeAfterstate(GameAction);  // returns a reward
	void addRandomTile(std::default_random_engine&);

	std::size_t getRandomTilePoint(std::default_random_engine&);
	int getRandomTileValue(std::default_random_engine&);

	int getTileValue(int tile) {
		return (stateInternal >> (tile << 2)) & UINT64_C(0xF);
	}

	void setTileValue(int tile, int value) {
		clearTileValue(tile);
		stateInternal |= (static_cast<uint64_t>(value) << (tile << 2));
	}

	void clearTileValue(int tile) {
		stateInternal &= ~(UINT64_C(0xF) << (tile << 2));
	}

private:
	void updateEmptyPoints(std::vector<std::size_t>& emptyPoints);

	uint64_t stateInternal;

	static const double TILE_2_PROBABILITY;
};

#endif  // SRC_COMMON_GAME_STATE_H_
