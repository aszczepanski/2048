#include "common/GameState.h"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

const double GameState::TILE_2_PROBABILITY = 0.9;

void GameState::initialize(default_random_engine& randomEngine) {
	stateInternal = 0;
	addRandomTile(randomEngine);
	addRandomTile(randomEngine);
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

int GameState::computeAfterstate(GameAction action) {
	assert(isActionEnabled(action));

	int reward = 0;

	if (action == LEFT) {
		for (int i=0; i<4; i++) {
			int nonZeros = 0;
			int val = 0, nextVal = 0, newVal = 0;
			for (int j=0; j<3; j++) {
				val = getTileValue(i*4 + j);
				if (val == 0) continue;

				int jj = j;
				for (; jj<3; jj++) {
					nextVal = getTileValue(i*4 + jj + 1);
					if (nextVal) {
						jj++;
						break;
					}
				}
				if (val == nextVal) {
					newVal = val + 1;
					reward += (1<<newVal);
					clearTileValue(i*4 + j);
					clearTileValue(i*4 + jj);
					setTileValue((i*4 + nonZeros++), newVal);
				} else {
					clearTileValue(i*4 + j);
					clearTileValue(i*4 + jj);
					setTileValue((i*4 + nonZeros++), val);
					setTileValue((i*4 + nonZeros), nextVal);
					j = nonZeros - 1;
				}
			}
			if ((val = getTileValue(i*4 + 3)) > 0) {
				clearTileValue(i*4 + 3);
				setTileValue((i*4 + nonZeros), val);
			}
		}
	} else if (action == RIGHT) {
		for (int i=0; i<4; i++) {
			int nonZeros = 3;
			int val = 0, nextVal = 0, newVal = 0;
			for (int j=3; j>=1; j--) {
				val = getTileValue(i*4 + j);
				if (val == 0) continue;

				int jj = j;
				for (; jj>=1; jj--) {
					nextVal = getTileValue(i*4 + (jj - 1));
					if (nextVal) {
						jj--;
						break;
					}
				}
				if (val == nextVal) {
					newVal = val + 1;
					reward += (1<<newVal);
					clearTileValue(i*4 + j);
					clearTileValue(i*4 + jj);
					setTileValue((i*4 + nonZeros--), newVal);
				} else {
					clearTileValue(i*4 + j);
					clearTileValue(i*4 + jj);
					setTileValue((i*4 + nonZeros--), val);
					setTileValue((i*4 + nonZeros), nextVal);
					j = nonZeros + 1;
				}
			}
			if ((val = getTileValue(i*4)) > 0) {
				clearTileValue(i*4);
				setTileValue((i*4 + nonZeros), val);
			}
		}
	} else if (action == UP) {
		for (int i=0; i<4; i++) {
			int nonZeros = 0;
			int val = 0, nextVal = 0, newVal = 0;
			for (int j=0; j<3; j++) {
				val = getTileValue(i + j*4);
				if (val == 0) continue;

				int jj = j;
				for (; jj<3; jj++) {
					nextVal = getTileValue(i + (jj+1)*4);
					if (nextVal) {
						jj++;
						break;
					}
				}
				if (val == nextVal) {
					newVal = val + 1;
					reward += (1<<newVal);
					clearTileValue(i + j*4);
					clearTileValue(i + jj*4);
					setTileValue((i + 4*nonZeros++), newVal);
				} else {
					clearTileValue(i + j*4);
					clearTileValue(i + jj*4);
					setTileValue((i + 4*nonZeros++), val);
					setTileValue((i + 4*nonZeros), nextVal);
					j = nonZeros - 1;
				}
			}
			if ((val = getTileValue(i + 3*4)) > 0) {
				clearTileValue(i + 3*4);
				setTileValue((i + nonZeros*4), val);
			}
		}
	} else {
		for (int i=0; i<4; i++) {
			int nonZeros = 3;
			int val = 0, nextVal = 0, newVal = 0;
			for (int j=3; j>=1; j--) {
				val = getTileValue(i + j*4);
				if (val == 0) continue;

				int jj = j;
				for (; jj>=1; jj--) {
					nextVal = getTileValue(i + (jj-1)*4);
					if (nextVal) {
						jj--;
						break;
					}
				}
				if (val == nextVal) {
					newVal = val + 1;
					reward += (1<<newVal);
					clearTileValue(i + j*4);
					clearTileValue(i + jj*4);
					setTileValue((i + 4*nonZeros--), newVal);
				} else {
					clearTileValue(i + j*4);
					clearTileValue(i + jj*4);
					setTileValue((i + 4*nonZeros--), val);
					setTileValue((i + 4*nonZeros), nextVal);
					j = nonZeros + 1;
				}
			}
			if ((val = getTileValue(i)) > 0) {
				clearTileValue(i);
				setTileValue((i + nonZeros*4), val);
			}
		}
	}

	return reward;
}

void GameState::addRandomTile(default_random_engine& randomEngine) {
	size_t randomTilePoint = getRandomTilePoint(randomEngine);
	int randomTileValue = getRandomTileValue(randomEngine);

	cout << randomTilePoint << " " << randomTileValue << endl;

	setTileValue(randomTilePoint, randomTileValue);
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
