#ifndef SRC_COMMON_GAME_STATS_H_
#define SRC_COMMON_GAME_STATS_H_

#include <chrono>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/count.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/median.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include "common/GameState.h"

class GameStatsContainer;

class GameStats {
public:
	typedef std::chrono::duration<int, std::milli> DurationType;

	GameStats* setScore(int score);
	GameStats* setMoves(int moves);
	GameStats* setDuration(DurationType duration);
	GameStats* setStage(uint16_t stage);

	friend std::ostream& operator<<(std::ostream&, const GameStats&);
	friend std::ostream& operator<<(std::ostream&, const GameStatsContainer&);
	friend class GameStatsContainer;

private:
	int score;
	int moves;
	DurationType duration;
	std::size_t stage;

	static std::string stageToString(uint32_t stage);
	const static std::vector<uint16_t> gameStages;
};

class GameStatsContainer {
public:
	GameStatsContainer();

	void addGameStats(const GameStats*);

	float getScoreMean() const;
	float getScore95ConfidenceInterval() const;
	float getStageCumulativeDistribution(std::size_t stage) const;
	float getDurationMean() const;
	float getMovesPerSecMean() const;

	friend std::ostream& operator<<(std::ostream&, const GameStatsContainer&);

private:
	boost::accumulators::accumulator_set<uint64_t, boost::accumulators::stats<boost::accumulators::tag::count, boost::accumulators::tag::mean, boost::accumulators::tag::variance> > scoreAccumulators;
	boost::accumulators::accumulator_set<uint64_t, boost::accumulators::stats<boost::accumulators::tag::mean, boost::accumulators::tag::sum> > durationAccumulators;
	boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::mean, boost::accumulators::tag::sum> > movesAccumulators;
	std::vector<boost::accumulators::accumulator_set<uint64_t, boost::accumulators::stats<boost::accumulators::tag::mean> > > stagesAccumulators;
};

#endif  // SRC_COMMON_GAME_STATS_H_
