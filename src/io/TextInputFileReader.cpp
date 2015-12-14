#include "io/TextInputFileReader.h"

#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

using namespace std;

shared_ptr<TuplesDescriptor> TextInputFileReader::read(const std::string& strategy, bool) {
	shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor = make_shared<ExpandedTuplesDescriptor>();

	auto startTimePoint = chrono::system_clock::now();

	tuplesDescriptor->stageBits = 0;
	tuplesDescriptor->stageBitsOffset = 16-0;
	tuplesDescriptor->tuples.resize(1<<0);

	fstream file;
	file.open(strategy, ios::in);
	if (file.good()) {
		cout << "Text input file was successfully opened." << endl;

		char c;
		file >> c;
		assert(c == '{');

		for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
			int T;
			file >> T;
			tuplesDescriptor->tuples[s].resize(T);
			for (int i=0; i<T; i++) {
				file >> c;
				assert(c == '{');
				ExpandedTuple* t = &tuplesDescriptor->tuples[s][i];
				int m, n;
				file >> n >> m;
				t->pts.resize(m);

				for (int j=0; j<m; j++) {
					file >> c;
					assert(c == '{');
					t->pts[j].resize(n);
					for (int k=0; k<n; k++) {
						int point;
						file >> point;
						t->pts[j][k] = point;
					}
					file >> c;
					assert(c == '}');
				}

				file >> c;
				assert(c == '{');
				t->lut.resize(1 << 4*n);
				for (int j=0; j<(1 << 4*n); j++) {
					file >> t->lut[j];
				}
				file >> c;
				assert(c == '}');
				file >> c;
				assert(c == '}');
			}
		}

		auto endTimePoint = chrono::system_clock::now();
		auto duration = chrono::duration_cast< chrono::duration<float> >
			(endTimePoint - startTimePoint);

		cout << "Strategy file was successfully read in "
			<< fixed << setprecision(2) << duration.count()
			<< " seconds." << endl;
	} else {
		cout << "Error while opening an input file." << endl;
	}

	file.close();


	return tuplesDescriptor;
}
