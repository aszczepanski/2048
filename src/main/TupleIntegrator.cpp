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
	// printTuplePoints();

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		cout << "stage " << s << endl;
		int T = tuplesDescriptor->tuples[s].size();
		bool found = true;
		while (found) {
			found = false;
			for (int i=0; i<T && !found; i++) {
				ExpandedTuple* ti = &tuplesDescriptor->tuples[s][i];
				for (int j=0; j<T; j++) {
					ExpandedTuple* tj = &tuplesDescriptor->tuples[s][j];
					if (i==j || ti->pts[0].size() > tj->pts[0].size()) continue;

					unique_ptr<vector<int>> matching = move(findMatch(ti, tj));
					if (matching) {
						cout << "matching " << i << "->" << j << endl;
						// for (int el : *matching) cout << el << " "; cout << endl;

						// add one to another
						for (int jp=0; jp<tj->lut.size(); jp++) {
							int ip = 0;
							for (int mm=0; mm<matching->size(); mm++) {
								if (matching->at(mm) == -1) continue;

								ip |= (((jp>>((matching->size()-mm-1)*4)) & 0xF)
									<< (4*(ti->pts[0].size()-matching->at(mm)-1)));
							}
							tj->lut[jp] += ti->lut[ip];
						}

						found = true;
						tuplesDescriptor->tuples[s].erase(tuplesDescriptor->tuples[s].begin()+i);
						T--;
						break;
					}
				}
			}
		}
	}
}

unique_ptr<vector<int>> TupleIntegrator::findMatch(ExpandedTuple* ti, ExpandedTuple* tj) {
	unique_ptr<vector<int>> matching(new vector<int>(tj->pts[0].size()));
	const int ipts = 0;

	for (int jpts=0; jpts<tj->pts.size(); jpts++) {
		fill(matching->begin(), matching->end(), -1);

		int m,n;
		bool change = true;
		for (m=0; m<ti->pts[0].size() && change;) {
			change = false;
			for (n=0; n<tj->pts[0].size(); n++) {
				if (ti->pts[ipts][m] == tj->pts[jpts][n]) {
					assert(matching->at(n) == -1);
					matching->at(n) = m++;
					change = true;
					break;
				}
			}
		}

		if (m == ti->pts[0].size()) {
			if (ipts == ti->pts.size()-1) {
				return move(matching);
			} else {
				if (isMatchInternal(ti, tj, ipts+1, matching.get())) {
					return move(matching);
				}
			}
		}
	}

	return nullptr;
}

bool TupleIntegrator::isMatchInternal(ExpandedTuple* ti, ExpandedTuple* tj, int ipts, vector<int>* matching) {
	for (int jpts=0; jpts<tj->pts.size(); jpts++) {

		int m,n;
		bool change = true;
		for (m=0; m<ti->pts[0].size() && change;) {
			change = false;
			for (n=0; n<tj->pts[0].size(); n++) {
				if (ti->pts[ipts][m] == tj->pts[jpts][n]) {
					if (matching->at(n) == m) {
						m++;
						change = true;
						break;
					}
				}
			}
		}

		if (m == ti->pts[0].size()) {
			if (ipts == ti->pts.size()-1) {
				return true;
			} else if (isMatchInternal(ti, tj, ipts+1, matching)) {
				return true;
			}
		}
	}

	return false;
}

void TupleIntegrator::printTuplePoints() {
	for (int i=0; i<tuplesDescriptor->tuples[0].size(); i++) {
		ExpandedTuple* tuple = &tuplesDescriptor->tuples[0][i];
		cout << "Tuple " << i << endl;
		for (auto vec : tuple->pts) {
			for (auto el : vec) cout << (int)el << " ";
			cout << endl;
		}
	}
}
