#include "main/TupleIntegrator.h"

#include <iostream>
#include <memory>

#include <boost/program_options.hpp>

#include "common/TuplesDescriptor.h"

#include "io/BinaryInputFileReader.h"
#include "io/BinaryInputFileWriter.h"
#include "io/TextInputFileReader.h"

using namespace std;
namespace po = boost::program_options;

const string TupleIntegrator::DEFAULT_INPUT_FILENAME_VALUE = "data/2048_strategies/eval-function.bin.special";
const string TupleIntegrator::DEFAULT_OUTPUT_FILENAME_VALUE = "tmp_strategy.bin.special";
const bool TupleIntegrator::DEFAULT_COMPRESS_VALUE = true;

TupleIntegrator::TupleIntegrator() {
}

int TupleIntegrator::run(int argc, char** argv) {
	if (!processArguments(argc, argv)) {
		return EXIT_SUCCESS;
	}

	initializeInputFileReader(inputFilename);
	tuplesDescriptor = dynamic_pointer_cast<ExpandedTuplesDescriptor>(
		inputFileReader->read(inputFilename, true));

	integrateTuples();

	BinaryInputFileWriter fileWriter;
	fileWriter.write(outputFilename, tuplesDescriptor, compress);

	return EXIT_SUCCESS;
}

void TupleIntegrator::initializeInputFileReader(const string& fn) {
	if (fn.substr(fn.find_last_of(".") + 1) == "txt") {
		inputFileReader = make_shared<TextInputFileReader>();
	} else {
		inputFileReader = make_shared<BinaryInputFileReader>();
	}
}

bool TupleIntegrator::processArguments(int argc, char** argv) {
	try {
		po::options_description optionsDescription("Allowed options");
		optionsDescription.add_options()
			("help,h",
				"produce help message")
			("input",
				po::value<string>(&inputFilename)->default_value(DEFAULT_INPUT_FILENAME_VALUE),
				"input file")
			("output",
				po::value<string>(&outputFilename)->default_value(DEFAULT_OUTPUT_FILENAME_VALUE),
				"input file")
			("compress",
				po::value<bool>(&compress)->default_value(DEFAULT_COMPRESS_VALUE),
				"input file");

		po::variables_map variablesMap;
		po::store(po::parse_command_line(argc, argv, optionsDescription), variablesMap);
		po::notify(variablesMap);

		if (variablesMap.count("help")) {
			cout << optionsDescription << endl;
			return false;
		}

	} catch (po::error& e) {
		cerr << e.what() << endl;
		return false;
	}

	return true;
}

void TupleIntegrator::integrateTuples() {
	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		int T = tuplesDescriptor->tuples[s].size();
		bool found = true;
		while (found) {
			found = false;
			for (int i=0; i<T && !found; i++) {
				ExpandedTuple* ti = &tuplesDescriptor->tuples[s][i];
				for (int j=0; j<T; j++) {
					ExpandedTuple* tj = &tuplesDescriptor->tuples[s][j];
					if (i==j || ti->pts[0].size() > tj->pts[0].size()) continue;

					vector<int> matching(tj->pts[0].size());
					fill(matching.begin(), matching.end(), -1);
					//for (int k=0; k<ti->pts.size(); k++) {  // TODO: variable size, different order
						int m,n;
						for (m=0, n=0; m<ti->pts[0].size() && n<tj->pts[0].size();) {
							if (ti->pts[0][m] == tj->pts[0][n]) {
								matching[n] = m;
								m++;
							}
							n++;
						}
						if (m == ti->pts[0].size()) {
							cout << "matching " << i << "->" << j << endl;
							for (int el : matching) cout << el << " "; cout << endl;

							// add one to another
							for (int jp=0; jp<tj->lut.size(); jp++) {
								int ip = 0;
								for (int mm=0; mm<matching.size(); mm++) {
									if (matching[mm] == -1) continue;
									ip <<= 4;
									ip |= ((jp>>((matching.size()-mm-1)*4)) & 0xF);
								}
								tj->lut[jp] += ti->lut[ip];
							}

							found = true;
							tuplesDescriptor->tuples[s].erase(tuplesDescriptor->tuples[s].begin()+i);
							tuplesDescriptor->tuples[s].shrink_to_fit();
							T--;
							break;
						}
					//}
				}
			}
			// found = false;
		}
	}
}
