#include <cstdlib>
#include <iostream>
#include <memory>

#include "main/Game.h"

#include "common/ProgramOptions.h"
#include "io/CProgramOptionsReader.h"

#include "io/IInputFileReader.h"
#include "io/BasicInputFileReader.h"

using namespace std;

int main(int argc, char* argv[]) {
	cout << "Hello, 2048" << endl;

	CProgramOptionsReader programOptionsReader;
	shared_ptr<ProgramOptions> programOptions = programOptionsReader.read(argc, argv);

	Game game(programOptions);
	game.play();

	return EXIT_SUCCESS;
}
