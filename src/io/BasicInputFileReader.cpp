#include "io/BasicInputFileReader.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

using namespace std;

const string BasicInputFileReader::STRATEGIES_DIRECTORY = "data/2048_strategies/";
const string BasicInputFileReader::STRATEGIES_EXTENSION = ".bin.txt";

shared_ptr<TuplesDescriptor> BasicInputFileReader::read(const std::string& strategy) {
	shared_ptr<TuplesDescriptor> tuplesDescriptor = make_shared<TuplesDescriptor>();

	fstream file;
	file.open(STRATEGIES_DIRECTORY + strategy + STRATEGIES_EXTENSION, ios::in);
	if (file.good()) {
		cout << "File was successfully opened." << endl;

		char c;
		file >> c;
		assert(c == '{');

		file >> tuplesDescriptor->T;
		tuplesDescriptor->tuples.resize(tuplesDescriptor->T);

		for (int i=0; i<tuplesDescriptor->T; i++) {
			file >> c;
			assert(c == '{');
			Tuple* t = &tuplesDescriptor->tuples[i];
			file >> t->n >> t->m;
			t->pts.resize(t->m);

			for (int j=0; j<t->m; j++) {
				file >> c;
				assert(c == '{');
				t->pts[j].resize(t->n);
				for (int k=0; k<t->n; k++) {
					file >> t->pts[j][k];
				}
				file >> c;
				assert(c == '}');
			}

			file >> c;
			assert(c == '{');
			t->lut.resize(1 << 4*t->n);
			for (int j=0; j<(1 << 4*t->n); j++) {
				file >> t->lut[j];
			}
			file >> c;
			assert(c == '}');
			file >> c;
			assert(c == '}');
		}
		cout << "File was successfully read." << endl;
	} else {
		cout << "Error while opening an input file." << endl;
	}

	file.close();


	return tuplesDescriptor;
}
