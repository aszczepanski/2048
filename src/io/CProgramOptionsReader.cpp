#include "io/CProgramOptionsReader.h"

#include <memory>
#include <string>

#include "common/ProgramOptions.h"

using namespace std;

shared_ptr<ProgramOptions> CProgramOptionsReader::read(int argc, char* argv[]) {
	shared_ptr<ProgramOptions> programOptions = make_shared<ProgramOptions>();

	// TODO: implement it!

	programOptions->strategy = "2048_a_weak_player";
	programOptions->randomSeed = 23862;
	programOptions->games = 1;
	programOptions->maxRoundTime = 1;
	programOptions->maxDepth = 4;
	programOptions->threads = 1;

	return programOptions;
}
