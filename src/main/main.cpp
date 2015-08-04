#include <cstdlib>
#include <iostream>
#include <memory>

#include "main/Game.h"

#include "common/GameState.h"
#include "common/GameStats.h"

#include "common/ProgramOptions.h"
#include "io/CProgramOptionsReader.h"

#include "common/TuplesDescriptor.h"
#include "io/IInputFileReader.h"
#include "io/BasicInputFileReader.h"

using namespace std;

int main(int argc, char* argv[]) {
	CProgramOptionsReader programOptionsReader;
	shared_ptr<ProgramOptions> programOptions = programOptionsReader.read(argc, argv);
	if (programOptions == nullptr) return EXIT_SUCCESS;

	BasicInputFileReader inputFileReader;
	shared_ptr<TuplesDescriptor> tuplesDescriptor = inputFileReader.read(programOptions->strategy);

	GameState::initializeTables();

	Game game(programOptions, tuplesDescriptor);

	GameStatsContainer gameStatsContainer;
	float avg = 0.0;

	for (unsigned i=0; i<programOptions->games; i++) {
		unique_ptr<GameStats> gameStats = move(game.play());
		avg += gameStats->score;
		cout << *gameStats << endl;
		// cout << i << ". score: " << gameStats->score << " avg: " << avg / (float)(i+1) << endl;
		gameStatsContainer.addGameStats(gameStats.get());
	}

	cout << gameStatsContainer << endl;

	return EXIT_SUCCESS;
}
