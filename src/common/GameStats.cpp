#include "common/GameStats.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstdint>
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

const vector<uint16_t> GameStats::gameStages = {
	1<<15 | 1<<14 | 1<<13 | 1<<12,
	1<<15 | 1<<14 | 1<<13,
	1<<15 | 1<<14 | 1<<12,
	1<<15 | 1<<14,
	1<<15 | 1<<13 | 1<<12,
	1<<15 | 1<<13,
	1<<15 | 1<<12,
	1<<15,
	1<<14 | 1<<13 | 1<<12,
	1<<14 | 1<<13,
	1<<14 | 1<<12,
	1<<14,
	1<<13 | 1<<12,
	1<<13,
	1<<12,
	1<<11,
	1<<10,
	1<<9,
	1<<8,
	1<<7,
	1<<6,
	1<<5,
	1<<4,
	1<<3,
	1<<2,
	1<<1,
	1<<0
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

GameStats* GameStats::setStage(uint16_t stage) {
	for (size_t i=0; i<gameStages.size(); i++) {
		if (gameStages[i] <= stage) {
			this->stage = i;
			break;
		}
	}
	return this;
}

ostream& operator<<(ostream& out, const GameStats& gameStats) {
	return out
		<< gameStats.score
		// << ", " << gameStats.moves
		<< ", " << GameStats::stageToString(GameStats::gameStages[gameStats.stage])
		<< ", " << gameStats.duration.count();
}

string GameStats::stageToString(uint32_t stage) {
	assert(stage > 0);

	stringstream ss;
	bool empty = true;
	
	for (int i=15; i>=0; i--) {
		if (stage & (1<<i)) {
			if (!empty) ss << "_";
			if (i >= 10) {
				ss << (1<<(i-10)) << "k";
			} else {
				ss << (1<<i);
			}
			empty = false;
		}
	}

	return ss.str();
}

void GameStatsContainer::addGameStats(const GameStats* gameStatsPtr) {
	// TODO
	scoreAccumulators(gameStatsPtr->score);
	durationAccumulators(gameStatsPtr->duration.count());

	for (size_t i=0; i<GameStats::gameStages.size(); i++) {
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
	out << "perf";
	out << ", " << "conf";

	for (size_t i=0; i<16; i++) {
		out << ", " << GameStats::stageToString(GameStats::gameStages[i]);
	}

	out << ", " << "time";
	out << endl;

	///////////////////////

	out << gameStatsContainer.getScoreMean();
	out	<< ", " << gameStatsContainer.getScore95ConfidenceInterval();

	for (size_t i=0; i<16; i++) {
		out << ", " << gameStatsContainer.getStageCumulativeDistribution(i);
	}

	out	<< ", " << gameStatsContainer.getDurationMean();

	return out;
}
