#include "common/GameStats.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <memory>
#include <iostream>
#include <ostream>
#include <sstream>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include "common/GameState.h"

using namespace std;
using namespace boost::accumulators;

const uint8_t GameStats::gameStages[27][4] {
	{15, 14, 13, 12},
	{15, 14, 13},
	{15, 14, 12},
	{15, 14},
	{15, 13, 12},
	{15, 13},
	{15, 13},
	{15},
	{14, 13, 12},
	{14, 13},
	{14, 12},
	{14},
	{13, 12},  // TODO: check the order
	{13},
	{12},
	{11},
	{10},
	{9},
	{8},
	{7},
	{6},
	{5},
	{4},
	{3},
	{2},
	{1},
	{0}
};

GameStats* GameStats::setScore(int score) {
	this->score = score;
	return this;
}

GameStats* GameStats::setMoves(int moves) {
	this->moves = moves;
	return this;
}

GameStats* GameStats::setDuration(DurationType duration) {
	this->duration = duration;
	return this;
}

GameStats* GameStats::setStage(size_t stage) {
	this->stage = stage;
	return this;
}

ostream& operator<<(ostream& out, const GameStats& gameStats) {
	return out
		<< gameStats.score
		// << ", " << gameStats.moves
		<< ", " << GameStats::stageToString(gameStats.stage)
		<< ", " << gameStats.duration.count();
}

size_t GameStats::calculateStage(GameState gameState) {
	array<int, 16> tiles;
	for (size_t i=0; i<16; i++) {
		tiles[i] = gameState.getTileValue(i);
	}

	sort(tiles.begin(), tiles.end(), greater<int>());

	for (size_t i=0; i<27; i++) {
		bool satisfied = true;
		for (size_t j=0; j<4 && satisfied; j++) {
			if (tiles[j] < gameStages[i][j]) satisfied = false;
		}
		if (satisfied) return i;
	}

	return 26;
}

string GameStats::stageToString(size_t stage) {
	assert(stage < 27);

	stringstream ss;
	bool empty = true;
	
	for (int i=0; i<4; i++) {
		if (gameStages[stage][i] == 0) break;
		if (!empty) ss << "_";
		if (gameStages[stage][i] >= 10) {
			ss << (1<<(gameStages[stage][i]-10)) << "k";
		} else {
			ss << (1<<gameStages[stage][i]);
		}
		empty = false;
	}

	return ss.str();
}

void GameStatsContainer::addGameStats(const GameStats* gameStatsPtr) {
	// TODO
	scoreAccumulators(gameStatsPtr->score);
	durationAccumulators(gameStatsPtr->duration.count());

	for (size_t i=0; i<27; i++) {
		stagesAccumulators[i](gameStatsPtr->stage <= i ? 1 : 0);
	}
}

float GameStatsContainer::getScoreMean() const {
	return mean(scoreAccumulators);
}

float GameStatsContainer::getScore95ConfidenceInterval() const {
	return 1.96f * sqrt(variance(scoreAccumulators) / boost::accumulators::count(scoreAccumulators));
}

float GameStatsContainer::getStageCumulativeDistribution(size_t stage) const {
	return mean(stagesAccumulators[stage]);
}

float GameStatsContainer::getDurationMean() const {
	return mean(durationAccumulators);
}

ostream& operator<<(ostream& out, const GameStatsContainer& gameStatsContainer) {
	out << gameStatsContainer.getScoreMean();
	out	<< ", " << gameStatsContainer.getScore95ConfidenceInterval();

	for (size_t i=0; i<16; i++) {
		out << ", " << gameStatsContainer.getStageCumulativeDistribution(i);
	}

	out	<< ", " << gameStatsContainer.getDurationMean();

	return out;
}
