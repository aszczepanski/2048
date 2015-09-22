#include "io/TextInputFileReader.h"

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

using namespace std;

shared_ptr<TuplesDescriptor> TextInputFileReader::read(const std::string& strategy) {
	shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor = make_shared<ExpandedTuplesDescriptor>();

	tuplesDescriptor->stageBits = 0;
	tuplesDescriptor->tuples.resize(1<<0);
	tuplesDescriptor->T.resize(1<<0);

	fstream file;
	file.open(strategy, ios::in);
	if (file.good()) {
		cout << "Strategy file was successfully opened." << endl;

		char c;
		file >> c;
		assert(c == '{');

		for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
			file >> tuplesDescriptor->T[s];
			tuplesDescriptor->tuples[s].resize(tuplesDescriptor->T[s]);
			for (int i=0; i<tuplesDescriptor->T[s]; i++) {
				file >> c;
				assert(c == '{');
				ExpandedTuple* t = &tuplesDescriptor->tuples[s][i];
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
		}
		cout << "Strategy file was successfully read." << endl;
	} else {
		cout << "Error while opening an input file." << endl;
	}

	file.close();


	return tuplesDescriptor;
}
