#ifndef SRC_MAIN_MAIN_APPLICATION_H_
#define SRC_MAIN_MAIN_APPLICATION_H_

#include <memory>
#include <mutex>
#include <random>

#include "common/ProgramOptions.h"
#include "io/CProgramOptionsReader.h"

#include "io/IInputFileReader.h"

#include "common/GameStats.h"

class MainApplication {
public:
	MainApplication();

	// returns EXIT_SUCCESS or EXIT_FAILURE
	int run(int argc, char** argv);

private:
	void initializeInputFileReader();
	void playGamesAndPrintStats();

	CProgramOptionsReader programOptionsReader;
	std::shared_ptr<ProgramOptions> programOptions;

	std::shared_ptr<IInputFileReader> inputFileReader;

	std::shared_ptr<TuplesDescriptor> tuplesDescriptor;

	GameStatsContainer gameStatsContainer;
	std::mutex gameStatsContainerMutex;

	std::unique_ptr<std::default_random_engine> randomEngine;
	std::uniform_int_distribution<unsigned> uniformDist;
	std::mutex randomEngineMutex;

};

#endif  // SRC_MAIN_MAIN_APPLICATION_H_
