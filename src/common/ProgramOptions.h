#ifndef SRC_COMMON_PROGRAM_OPTIONS_H_
#define SRC_COMMON_PROGRAM_OPTIONS_H_

#include <string>

struct ProgramOptions {
	std::string strategy;
	bool unzip;
	unsigned randomSeed;
	unsigned games;
	unsigned maxRoundTime;
	unsigned maxDepth;
	bool evalMultithreading;
	unsigned gameThreads;
	bool verbose;
};

#endif  // SRC_COMMON_PROGRAM_OPTIONS_H_
