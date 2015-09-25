#include "common/GameState.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

const double GameState::TILE_2_PROBABILITY = 0.9;
const double GameState::TILE_4_PROBABILITY = 0.1;

uint16_t GameState::rowLeftTable[65536];
uint16_t GameState::rowRightTable[65536];
uint64_t GameState::colUpTable[65536];
uint64_t GameState::colDownTable[65536];

int GameState::scoreTable[65536];

uint16_t GameState::stageTable[65536];

unsigned GameState::initialize(default_random_engine& positionRandomEngine, default_random_engine& valueRandomEngine) {
	clear();

	unsigned scorePenalty = 0;
	scorePenalty += addRandomTile(positionRandomEngine, valueRandomEngine);
	scorePenalty += addRandomTile(positionRandomEngine, valueRandomEngine);

	return scorePenalty;
}

bool GameState::isTerminalState() {
	for (int i=0; i<4; i++) {
		for (int j=0; j<3; j++) {
			if (getTileValue(i*4 + j) == 0) return false;
			if (getTileValue(i*4 + j) == getTileValue(i*4 + (j+1))) return false;
			if (getTileValue(j*4 + i) == getTileValue((j+1)*4 + i)) return false;
		}
		if (getTileValue(i*4 + 3) == 0) return false;
	}
	return true;
}

bool GameState::isActionEnabled(GameAction action) {
	bool zeroInLine;
	if (action == LEFT) {
		for (int i=0; i<4; i++) {
			zeroInLine = false;
			for (int j=0; j<3; j++) {
				if (getTileValue(i*4 + j) == 0) {
					zeroInLine = true;
				} else if (zeroInLine) {
					return true;
				} else if (getTileValue(i*4 + j) == getTileValue(i*4 + (j+1))) {
					return true;
				}
			}
			if (getTileValue(i*4 + 3) > 0 && zeroInLine) return true;
		}
		return false;
	} else if (action == RIGHT) {
		for (int i=0; i<4; i++) {
			zeroInLine = false;
			for (int j=3; j>=1; j--) {
				if (getTileValue(i*4 + j) == 0) {
					zeroInLine = true;
				} else if (zeroInLine) {
					return true;
				} else if (getTileValue(i*4 + j) == getTileValue(i*4 + (j-1))) {
					return true;
				}
			}
			if (getTileValue(i*4) > 0 && zeroInLine) return true;
		}
		return false;
	} else if (action == UP) {
		for (int i=0; i<4; i++) {
			zeroInLine = false;
			for (int j=0; j<3; j++) {
				if (getTileValue(i + j*4) == 0) {
					zeroInLine = true;
				} else if (zeroInLine) {
					return true;
				} else if (getTileValue(i + j*4) == getTileValue(i + 4*(j+1))) {
					return true;
				}
			}
			if (getTileValue(i + 3*4) > 0 && zeroInLine) return true;
		}
		return false;
	} else {
		for (int i=0; i<4; i++) {
			zeroInLine = false;
			for (int j=3; j>=1; j--) {
				if (getTileValue(i + j*4) == 0) {
					zeroInLine = true;
				} else if (zeroInLine) {
					return true;
				} else if (getTileValue(i + j*4) == getTileValue(i + 4*(j-1))) {
					return true;
				}
			}
			if (getTileValue(i) > 0 && zeroInLine) return true;
		}
		return false;
	}
}

void GameState::computeAfterstate(GameAction action) {
	if (action == LEFT) {
		stateInternal ^= uint64_t(rowLeftTable[(stateInternal >>  0) & UINT64_C(0xFFFF)]) <<  0;
		stateInternal ^= uint64_t(rowLeftTable[(stateInternal >> 16) & UINT64_C(0xFFFF)]) << 16;
		stateInternal ^= uint64_t(rowLeftTable[(stateInternal >> 32) & UINT64_C(0xFFFF)]) << 32;
		stateInternal ^= uint64_t(rowLeftTable[(stateInternal >> 48) & UINT64_C(0xFFFF)]) << 48;
	} else if (action == RIGHT) {
		stateInternal ^= uint64_t(rowRightTable[(stateInternal >>  0) & UINT64_C(0xFFFF)]) <<  0;
		stateInternal ^= uint64_t(rowRightTable[(stateInternal >> 16) & UINT64_C(0xFFFF)]) << 16;
		stateInternal ^= uint64_t(rowRightTable[(stateInternal >> 32) & UINT64_C(0xFFFF)]) << 32;
		stateInternal ^= uint64_t(rowRightTable[(stateInternal >> 48) & UINT64_C(0xFFFF)]) << 48;
	} else if (action == UP) {
		uint64_t t = transpose(stateInternal);
		stateInternal ^= colUpTable[(t >>  0) & UINT64_C(0xFFFF)] <<  0;
		stateInternal ^= colUpTable[(t >> 16) & UINT64_C(0xFFFF)] <<  4;
		stateInternal ^= colUpTable[(t >> 32) & UINT64_C(0xFFFF)] <<  8;
		stateInternal ^= colUpTable[(t >> 48) & UINT64_C(0xFFFF)] << 12;
	} else {
		uint64_t t = transpose(stateInternal);
		stateInternal ^= colDownTable[(t >>  0) & UINT64_C(0xFFFF)] <<  0;
		stateInternal ^= colDownTable[(t >> 16) & UINT64_C(0xFFFF)] <<  4;
		stateInternal ^= colDownTable[(t >> 32) & UINT64_C(0xFFFF)] <<  8;
		stateInternal ^= colDownTable[(t >> 48) & UINT64_C(0xFFFF)] << 12;
	}
}

unsigned GameState::addRandomTile(default_random_engine& positionRandomEngine, default_random_engine& valueRandomEngine) {
	size_t randomTilePoint = getRandomTilePoint(positionRandomEngine);
	int randomTileValue = getRandomTileValue(valueRandomEngine);

	setTileValue(randomTilePoint, randomTileValue);

	return randomTileValue - 1;
}

size_t GameState::getRandomTilePoint(default_random_engine& randomEngine) {
	vector<size_t> emptyPoints;
	emptyPoints.reserve(16);
	updateEmptyPoints(emptyPoints);

	assert(!emptyPoints.empty());

	uniform_int_distribution<> dis(0, emptyPoints.size()-1);
	return emptyPoints[dis(randomEngine)];
}

int GameState::getRandomTileValue(default_random_engine& randomEngine) {
	uniform_real_distribution<double> dis(0.0, 1.0);
	if (dis(randomEngine) <= TILE_2_PROBABILITY) {
		return 1;
	}
	else {
		return 2;
	}
}

void GameState::updateEmptyPoints(vector<size_t>& emptyPoints) {
	emptyPoints.clear();
	for (int i=0; i<16; i++) {
		if (getTileValue(i) == 0) {
			emptyPoints.push_back(i);
		}
	}
}

int GameState::scoreBoard() {
	return scoreTable[(stateInternal >>  0) & UINT64_C(0xFFFF)]
		+ scoreTable[(stateInternal >> 16) & UINT64_C(0xFFFF)]
		+ scoreTable[(stateInternal >> 32) & UINT64_C(0xFFFF)]
		+ scoreTable[(stateInternal >> 48) & UINT64_C(0xFFFF)];
}

void GameState::initializeTables() {
	for (unsigned row=0; row<65536; row++) {
		unsigned line[4] = {
			(row >> 0) & 0xF,
			(row >> 4) & 0xF,
			(row >> 8) & 0xF,
			(row >> 12) & 0xF
		};

		// score
		int score = 0;
		for (int i=0; i<4; i++) {
			int rank = line[i];
			if (rank >= 2) {
				score += (rank-1) * (1<<rank);
			}
		}
		scoreTable[row] = score;

		// stage
		{
			stageTable[row] = 1<<line[0];
			uint16_t tile, tmp;
			tile = 1<<line[1];
			while (tile) {
				tmp = stageTable[row] & tile;
				stageTable[row] |= tile;
				tile = tmp >> 1;
			}
			tile = 1<<line[2];
			while (tile) {
				tmp = stageTable[row] & tile;
				stageTable[row] |= tile;
				tile = tmp >> 1;
			}
			tile = 1<<line[3];
			while (tile) {
				tmp = stageTable[row] & tile;
				stageTable[row] |= tile;
				tile = tmp >> 1;
			}
		}

		// execute a move to the left
		for (int i=0; i<3; i++) {
			int j;
			for (j=i+1; j<4; j++) {
				if (line[j] != 0) break;
			}
			if (j == 4) break;

			if (line[i] == 0) {
				line[i] = line[j];
				line[j] = 0;
				i--;
			} else if (line[i] == line[j]) {
				if(line[i] != 0xf) {
					line[i]++;  // 32768 + 32768 = 32768
				}
				line[j] = 0;
			}
		}

		uint16_t result = (line[0]<<0) | (line[1]<<4) | (line[2]<<8) | (line[3]<<12);
		uint16_t revResult = reverseRow(result);
		unsigned revRow = reverseRow(row);

		rowLeftTable[row] = row^result;
		rowRightTable[revRow] = revRow^revResult;
		colUpTable[row] = unpackCol(row)^unpackCol(result);
		colDownTable[revRow] = unpackCol(revRow)^unpackCol(revResult);
	}
}

uint16_t GameState::reverseRow(uint16_t row) {
	return (row >> 12) | ((row >> 4) & 0x00F0)  | ((row << 4) & 0x0F00) | (row << 12);
}

uint64_t GameState::unpackCol(uint16_t row) {
	uint64_t tmp = row;
	return (tmp | (tmp << UINT64_C(12)) | (tmp << UINT64_C(24)) | (tmp << UINT64_C(36)))
		& UINT64_C(0x000F000F000F000F);
}
