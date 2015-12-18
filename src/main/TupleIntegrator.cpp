#include "main/TupleIntegrator.h"

#include <iostream>
#include <memory>

#include "common/TuplesDescriptor.h"

#include "io/BinaryInputFileReader.h"
#include "io/BinaryInputFileWriter.h"
#include "io/TextInputFileReader.h"

using namespace std;

TupleIntegrator::TupleIntegrator() {
}

int TupleIntegrator::run(int argc, char** argv) {
	cout << "TupleIntegrator::run" << endl;

	const string readStrategy = "data/2048_strategies/eval-function.bin.special";
	const string writeStrategy = "tmpStrategy";

	initializeInputFileReader(readStrategy);
	tuplesDescriptor = dynamic_pointer_cast<ExpandedTuplesDescriptor>(
		inputFileReader->read(readStrategy, true));

	integrateTuples();

	BinaryInputFileWriter fileWriter;
	fileWriter.write(writeStrategy, tuplesDescriptor);

	return EXIT_SUCCESS;
}

void TupleIntegrator::initializeInputFileReader(const string& fn) {
	if (fn.substr(fn.find_last_of(".") + 1) == "txt") {
		inputFileReader = make_shared<TextInputFileReader>();
	} else {
		inputFileReader = make_shared<BinaryInputFileReader>();
	}
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
