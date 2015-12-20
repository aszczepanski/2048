#include "io/TextInputFileReader.h"

#include <array>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

#define NDEBUG
#include <cassert>

using namespace std;

shared_ptr<TuplesDescriptor> TextInputFileReader::read(const string& strategy, bool) {
	shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor = make_shared<ExpandedTuplesDescriptor>();

	auto startTimePoint = chrono::system_clock::now();

	initializeTuplesDescriptor(tuplesDescriptor);

	fstream file;
	file.open(strategy, ios::in);
	if (file.good()) {
		cout << "Text input file was opened and is being read." << endl;

		if (!readOpenedTextFile(file, tuplesDescriptor)) {
			cerr << "Error while reading and input file." << endl;
			return nullptr;
		}

		auto endTimePoint = chrono::system_clock::now();
		auto duration = chrono::duration_cast< chrono::duration<float> >
			(endTimePoint - startTimePoint);

		cout << "Strategy file was read in "
			<< fixed << setprecision(2) << duration.count()
			<< " seconds." << endl;
	} else {
		cerr << "Error while opening an input file." << endl;
	}

	file.close();


	return tuplesDescriptor;
}

void TextInputFileReader::initializeTuplesDescriptor(shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor) {
	tuplesDescriptor->stageBits = 0;
	tuplesDescriptor->stageBitsOffset = 16-0;
	tuplesDescriptor->tuples.resize(1<<0);
}

bool TextInputFileReader::readOpenedTextFile(fstream& file, shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor) {
	if (!consumeCharacter(file, '{')) return false;

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		if (!readStageDescription(file, tuplesDescriptor, s)) return false;
	}

	if (!consumeCharacter(file, '}')) return false;
	return true;
}

bool TextInputFileReader::readStageDescription(fstream& file, shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor, int s) {
	int T;
	file >> T;
	tuplesDescriptor->tuples[s].resize(T);
	for (int i=0; i<T; i++) {
		if (!consumeCharacter(file, '{')) return false;
		ExpandedTuple* t = &tuplesDescriptor->tuples[s][i];
		int m, n;
		file >> n >> m;
		t->pts.resize(m);

		for (int j=0; j<m; j++) {
			if (!consumeCharacter(file, '{')) return false;
			t->pts[j].resize(n);
			for (int k=0; k<n; k++) {
				int point;
				file >> point;
				t->pts[j][k] = point;
			}
			if (!consumeCharacter(file, '}')) return false;
		}

		if (!consumeCharacter(file, '{')) return false;
		t->lut.resize(1 << 4*n);
		for (int j=0; j<(1 << 4*n); j++) {
			file >> t->lut[j];
		}
		if (!consumeCharacter(file, '}')) return false;
		if (!consumeCharacter(file, '}')) return false;
	}

	return true;
}

bool TextInputFileReader::consumeCharacter(fstream& file, char requested) {
	char c;
	file >> c;
	assert(c == requested);
	return (c == requested);
}
