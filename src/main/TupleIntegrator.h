#ifndef SRC_MAIN_TUPLE_INTEGRATOR_H_
#define SRC_MAIN_TUPLE_INTEGRATOR_H_

#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"
#include "io/IInputFileReader.h"

class TupleIntegrator {
public:
	TupleIntegrator();

	// return EXIT_SUCCESS or EXIT_FAILURE
	int run(int argc, char** argv);

private:
	void initializeInputFileReader(const std::string& fn);
	bool processArguments(int argc, char** argv);

	void integrateTuples();

	std::unique_ptr<std::vector<int>> findMatch(ExpandedTuple* ti, ExpandedTuple* tj);
	bool isMatchInternal(ExpandedTuple* ti, ExpandedTuple* tj, int ipts, std::vector<int>* matching);

	void printTuplePoints();

	std::string inputFilename;
	std::string outputFilename;
	bool compress;

	std::shared_ptr<IInputFileReader> inputFileReader;
	std::shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor;

	const static std::string DEFAULT_INPUT_FILENAME_VALUE;
	const static std::string DEFAULT_OUTPUT_FILENAME_VALUE;
	const static bool DEFAULT_COMPRESS_VALUE;
};

#endif  // SRC_MAIN_TUPLE_INTEGRATOR_H_
