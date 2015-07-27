#include "io/CProgramOptionsReader.h"

#include <chrono>
#include <memory>
#include <string>

#include "common/ProgramOptions.h"

using namespace std;

shared_ptr<ProgramOptions> CProgramOptionsReader::read(int argc, char* argv[]) {
	shared_ptr<ProgramOptions> programOptions = make_shared<ProgramOptions>();

	// TODO: implement it!

	programOptions->strategy = "2048_a_weak_player";
	// programOptions->strategy = "2048_entry_level";
	programOptions->randomSeed = chrono::system_clock::now().time_since_epoch().count();
	// programOptions->randomSeed = 23862;
	programOptions->games = 1;
	programOptions->maxRoundTime = 1;
	programOptions->maxDepth = 3;
	programOptions->threads = 1;

	return programOptions;
}
