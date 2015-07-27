#include <cstdlib>
#include <iostream>
#include <memory>

#include "main/Game.h"

#include "common/ProgramOptions.h"
#include "io/CProgramOptionsReader.h"

#include "common/TuplesDescriptor.h"
#include "io/IInputFileReader.h"
#include "io/BasicInputFileReader.h"

using namespace std;

int main(int argc, char* argv[]) {
	cout << "Hello, 2048" << endl;

	CProgramOptionsReader programOptionsReader;
	shared_ptr<ProgramOptions> programOptions = programOptionsReader.read(argc, argv);

	BasicInputFileReader inputFileReader;
	shared_ptr<TuplesDescriptor> tuplesDescriptor = inputFileReader.read(programOptions->strategy);

	Game game(programOptions, tuplesDescriptor);

	float avg = 0.0;

	for (int i=0; i<programOptions->games; i++) {
		int score = game.play();
		avg += score;
		cout << i << ". score: " << score << " avg: " << avg / (float)(i+1) << endl;
	}

	cout << "avg: " << avg / static_cast<float>(programOptions->games) << endl;

	return EXIT_SUCCESS;
}
