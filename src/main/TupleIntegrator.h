#ifndef SRC_MAIN_TUPLE_INTEGRATOR_H_
#define SRC_MAIN_TUPLE_INTEGRATOR_H_

#include <memory>

#include "common/TuplesDescriptor.h"
#include "io/IInputFileReader.h"

class TupleIntegrator {
public:
	TupleIntegrator();

	// return EXIT_SUCCESS or EXIT_FAILURE
	int run(int argc, char** argv);

private:
	void initializeInputFileReader(const std::string& fn);

	void integrateTuples();

	std::shared_ptr<IInputFileReader> inputFileReader;
	std::shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor;
};

#endif  // SRC_MAIN_TUPLE_INTEGRATOR_H_
