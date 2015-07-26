#ifndef SRC_COMMON_PROGRAM_OPTIONS_H_
#define SRC_COMMON_PROGRAM_OPTIONS_H_

#include <string>

struct ProgramOptions {
	std::string strategy;
	unsigned randomSeed;
	unsigned games;  // TODO: delete it
	unsigned maxRoundTime;
	unsigned maxDepth;
	unsigned threads;
};

#endif  // SRC_COMMON_PROGRAM_OPTIONS_H_
