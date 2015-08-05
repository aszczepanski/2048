#include "io/CProgramOptionsReader.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include <boost/program_options.hpp>

#include "common/ProgramOptions.h"

using namespace std;
namespace po = boost::program_options;

const string CProgramOptionsReader::DEFAULT_STRATEGY_VALUE = "data/2048_strategies/2048_a_weak_player.bin.txt";
const unsigned CProgramOptionsReader::DEFAULT_GAMES_VALUE = 1;
const unsigned CProgramOptionsReader::DEFAULT_TIME_VALUE = 1;
const unsigned CProgramOptionsReader::DEFAULT_DEPTH_VALUE = 1;
const unsigned CProgramOptionsReader::DEFAULT_THREADS_VALUE = 1;

shared_ptr<ProgramOptions> CProgramOptionsReader::read(int argc, char* argv[]) {
	shared_ptr<ProgramOptions> programOptions = make_shared<ProgramOptions>();

	// TODO: handle exceptions

	po::options_description optionsDescription("Allowed options");
	optionsDescription.add_options()
		("help,h",
			"produce help message")
		("strategy",
			po::value<string>(&programOptions->strategy)->default_value(DEFAULT_STRATEGY_VALUE),
			"strategy input file")
		("seed",
			po::value<unsigned>(&programOptions->randomSeed)
				->default_value(chrono::system_clock::now().time_since_epoch().count()),
			"random seed")
		("games",
			po::value<unsigned>(&programOptions->games)->default_value(DEFAULT_GAMES_VALUE),
			"number of games")
		("time",
			po::value<unsigned>(&programOptions->maxRoundTime)->default_value(DEFAULT_TIME_VALUE),
			"maximum time for one round")
		("depth",
			po::value<unsigned>(&programOptions->maxDepth)->default_value(DEFAULT_DEPTH_VALUE),
			"maximum depth for expectimax")
		("threads",
			po::value<unsigned>(&programOptions->threads)->default_value(DEFAULT_THREADS_VALUE),
			"number of threads")
		("verbose,v",
			"show board and score after each round");

	po::variables_map variablesMap;
	po::store(po::parse_command_line(argc, argv, optionsDescription), variablesMap);
	po::notify(variablesMap);

	if (variablesMap.count("help")) {
		cout << optionsDescription << endl;
		return nullptr;
	}

	programOptions->verbose = variablesMap.count("verbose");

	return programOptions;
}
