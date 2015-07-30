#include <cstdlib>
#include <iostream>
#include <memory>

#include "main/Game.h"

#include "common/GameState.h"

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

	float avg = 0.0;

	for (unsigned i=0; i<programOptions->games; i++) {
		int score = game.play();
		avg += score;
		cout << i << ". score: " << score << " avg: " << avg / (float)(i+1) << endl;
	}

	cout << "avg: " << avg / static_cast<float>(programOptions->games) << endl;

	return EXIT_SUCCESS;
}
