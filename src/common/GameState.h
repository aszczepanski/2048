#ifndef SRC_COMMON_GAME_STATE_H_
#define SRC_COMMON_GAME_STATE_H_

// Some of the ideas connected with the board's representation
// are based on https://github.com/nneonneo/2048-ai

#include <cstddef>
#include <cstdint>
#include <random>

#include "common/GameAction.h"

class GameState {
public:
	GameState(uint64_t board = UINT64_C(0)) : stateInternal(board) {}
	GameState(const GameState&) = default;
	GameState& operator=(const GameState&) = default;

	unsigned initialize(std::default_random_engine& positionRandomEngine,
		std::default_random_engine& valueRandomEngine);  // returns a number of fours
	bool isTerminalState();
	bool isActionEnabled(GameAction);
	void computeAfterstate(GameAction);
	void computeAfterstateAndUpdateOverflow(GameAction, bool* isStageOverflow);
	unsigned addRandomTile(std::default_random_engine& positionRandomEngine,
		std::default_random_engine& valueRandomEngine);  // returns a number of fours

	void clear() {
		stateInternal = UINT64_C(0);
	}

	std::size_t getRandomTilePoint(std::default_random_engine&);
	int getRandomTileValue(std::default_random_engine&);

	uint8_t getTileValue(uint8_t tile) {
		return (stateInternal >> (tile << 2)) & UINT64_C(0xF);
	}

	void setTileValue(uint8_t tile, uint8_t value) {
		clearTileValue(tile);
		stateInternal |= (static_cast<uint64_t>(value) << (tile << 2));
	}

	void clearTileValue(uint8_t tile) {
		stateInternal &= ~(UINT64_C(0xF) << (tile << 2));
	}

	uint16_t calculateStage(uint8_t offset = UINT8_C(0)) {
		uint16_t stage = UINT16_C(0);
		uint16_t tmp;

		auto calc = [&stage, &tmp] (uint16_t rowStage) {
			while (rowStage) {
				tmp = stage & rowStage;
				stage |= rowStage;
				rowStage = tmp >> 1;
			}
		};

		calc(stageTable[(stateInternal >>  0) & UINT64_C(0xFFFF)] >> offset);
		calc(stageTable[(stateInternal >> 16) & UINT64_C(0xFFFF)] >> offset);
		calc(stageTable[(stateInternal >> 32) & UINT64_C(0xFFFF)] >> offset);
		calc(stageTable[(stateInternal >> 48) & UINT64_C(0xFFFF)] >> offset);

		return stage;
	}

	uint8_t countEmpty() {
		uint64_t board = stateInternal;

		board |= (board >> 2) & UINT64_C(0x3333333333333333);
		board |= (board >> 1);
		board = ~board & UINT64_C(0x1111111111111111);

		board += board >> 32;
		board += board >> 16;
		board += board >> 8;
		board += board >> 4;
		return board & 0xF;
	}

	int scoreBoard();

	bool operator==(const GameState& gameState) {
		return stateInternal == gameState.stateInternal;
	}

	operator uint64_t() const {
		return stateInternal;
	}

	static const double TILE_2_PROBABILITY;
	static const double TILE_4_PROBABILITY;

	static void initializeTables();

private:
	void updateEmptyPoints(std::vector<std::size_t>& emptyPoints);

	uint64_t stateInternal;

	static uint16_t reverseRow(uint16_t row);
	static uint64_t unpackCol(uint16_t row);

	static uint64_t transpose(uint64_t board) {
		uint64_t a1 = board & UINT64_C(0xF0F00F0FF0F00F0F);
		uint64_t a2 = board & UINT64_C(0x0000F0F00000F0F0);
		uint64_t a3 = board & UINT64_C(0x0F0F00000F0F0000);
		uint64_t a = a1 | (a2 << 12) | (a3 >> 12);
		uint64_t b1 = a & UINT64_C(0xFF00FF0000FF00FF);
		uint64_t b2 = a & UINT64_C(0x00FF00FF00000000);
		uint64_t b3 = a & UINT64_C(0x00000000FF00FF00);
		return b1 | (b2 >> 24) | (b3 << 24);
	}

	static uint16_t rowLeftTable[65536];
	static uint16_t rowRightTable[65536];
	static uint64_t colUpTable[65536];
	static uint64_t colDownTable[65536];

	static int scoreTable[65536];

	static uint16_t stageTable[65536];
	static bool stageOverflow[65536];
};

#endif  // SRC_COMMON_GAME_STATE_H_
