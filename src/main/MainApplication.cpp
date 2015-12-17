#include "main/MainApplication.h"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <random>

#include "main/Game.h"

#include "common/GameState.h"
#include "common/GameStats.h"

#include "common/ProgramOptions.h"
#include "io/CProgramOptionsReader.h"

#include "common/TuplesDescriptor.h"
#include "io/IInputFileReader.h"
#include "io/BinaryInputFileReader.h"
#include "io/TextInputFileReader.h"

using namespace std;

MainApplication::MainApplication() {
}

int MainApplication::run(int argc, char** argv) {

	programOptions = programOptionsReader.read(argc, argv);
	if (programOptions == nullptr) return EXIT_SUCCESS;

	initializeInputFileReader();
	tuplesDescriptor = inputFileReader->read(
		programOptions->strategy, programOptions->unzip);
	if (tuplesDescriptor == nullptr) return EXIT_SUCCESS;

	GameState::initializeTables();

	randomEngine = move(unique_ptr<default_random_engine>(
		new default_random_engine(programOptions->randomSeed)));

	playGamesAndPrintStats();

	return EXIT_SUCCESS;
}

void MainApplication::initializeInputFileReader() {
	string fn = programOptions->strategy;
	if (fn.substr(fn.find_last_of(".") + 1) == "txt") {
		inputFileReader = make_shared<TextInputFileReader>();
	} else {
		inputFileReader = make_shared<BinaryInputFileReader>();
	}
}

void MainApplication::playGamesAndPrintStats() {
	cout << GameStats::header() << endl;

	atomic_int gamesCounter(programOptions->games);
	vector<thread> gameWorkers(programOptions->gameThreads);
	for (size_t i=0; i<gameWorkers.size(); i++) {
		gameWorkers[i] = thread([&](){
			unsigned seed1, seed2;
			int gc;
			while ((gc = --gamesCounter) >= 0) {
				{
					lock_guard<mutex> lock(randomEngineMutex);
					seed1 = uniformDist(*randomEngine);
					seed2 = uniformDist(*randomEngine);
				}
				Game game(programOptions, tuplesDescriptor, seed1, seed2); 

				unique_ptr<GameStats> gameStats = move(game.play());
				lock_guard<mutex> lock(gameStatsContainerMutex);
				cout << *gameStats << endl;
				// cout << gc << ": " << *gameStats << endl;
				gameStatsContainer.addGameStats(gameStats.get());
			}
		});
	}

	for (size_t i=0; i<gameWorkers.size(); i++) {
		gameWorkers[i].join();
	}
	
	cout << gameStatsContainer << endl;
}
