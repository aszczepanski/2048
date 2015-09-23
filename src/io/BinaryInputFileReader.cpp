#include "io/BinaryInputFileReader.h"

#include <array>
#include <cassert>
#include <cfloat>
#include <climits>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

using namespace std;

shared_ptr<TuplesDescriptor> BinaryInputFileReader::read(const string& strategy) {
	shared_ptr<TuplesDescriptor> tuplesDescriptor;

	fstream file;
	file.open(strategy, ios::in | ios::binary);

	if (file.good()) {
		cout << "Strategy file was successfully opened." << endl;

		unsigned char buf[4];

		int fileType;
		file.read((char*)buf, 4);
		fileType = convertIntLittleEndian(buf);

		int values;
		file.read((char*)buf, 4);
		values = convertIntLittleEndian(buf);
		assert(values == 16);

		if (fileType == 1) {
			tuplesDescriptor = readCompressedFile(file);
		} else {
			tuplesDescriptor = readExpandedFile(file);
		}

		cout << "Strategy file was successfully read." << endl;
	} else {
		cout << "Error while opening an input file." << endl;
	}

	file.close();

	return tuplesDescriptor;
}

shared_ptr<ExpandedTuplesDescriptor> BinaryInputFileReader::readExpandedFile(fstream& file) {
	shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor = make_shared<ExpandedTuplesDescriptor>();

	tuplesDescriptor->stageBits = readIntLittleEndian(file);
	tuplesDescriptor->tuples.resize(1<<tuplesDescriptor->stageBits);

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		int T = readIntLittleEndian(file);
		tuplesDescriptor->tuples[s].resize(T);
		for (int i=0; i<T; i++) {
			ExpandedTuple* t = &tuplesDescriptor->tuples[s][i];
			int n = readIntLittleEndian(file);
			int m = readIntLittleEndian(file);
			t->pts.resize(m);

			for (int j=0; j<m; j++) {
				t->pts[j].resize(n);
				for (int k=0; k<n; k++) {
					t->pts[j][k] = readIntLittleEndian(file);
				}
			}

			t->lut.resize(1 << 4*n);
			for (int j=0; j<(1 << 4*n); j++) {
				t->lut[j] = readFloatLittleEndian(file);
			}
		}
	}

	return tuplesDescriptor;
}

shared_ptr<CompressedTuplesDescriptor> BinaryInputFileReader::readCompressedFile(fstream& file) {
	shared_ptr<CompressedTuplesDescriptor> tuplesDescriptor = make_shared<CompressedTuplesDescriptor>();

	shared_ptr< array<TupleValueType, 16> > nullArray = make_shared< array<TupleValueType, 16> >();
	nullArray->fill(0);

	unsigned char buf[4];

	tuplesDescriptor->stageBits = readIntLittleEndian(file);
	tuplesDescriptor->tuples.resize(1<<tuplesDescriptor->stageBits);

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		int T = readIntLittleEndian(file);
		tuplesDescriptor->tuples[s].resize(T);
		for (int i=0; i<T; i++) {
			CompressedTuple* t = &tuplesDescriptor->tuples[s][i];
			int n = readIntLittleEndian(file);
			int m = readIntLittleEndian(file);
			t->pts.resize(m);

			for (int j=0; j<m; j++) {
				t->pts[j].resize(n);
				for (int k=0; k<n; k++) {
					t->pts[j][k] = readIntLittleEndian(file);
				}
			}

			t->lutPtrs.resize(1 << 4*(n-1));
			for (int j=0; j<(1 << 4*(n-1)); j++) {
				file.read((char*)buf, 4);
				if (isNegativeInfinity(buf)) {
					t->lutPtrs[j] = nullArray;
				} else {
					float firstVal = convertFloatLittleEndian(buf);
					t->lutPtrs[j] = make_shared<array<TupleValueType, 16> >();
					t->lutPtrs[j]->at(0) = firstVal;
					for (int k=1; k<16; k++) {
						t->lutPtrs[j]->at(k) = readFloatLittleEndian(file);
					}
				}
			}
		}
	}

	return tuplesDescriptor;
}

int32_t BinaryInputFileReader::convertIntLittleEndian(unsigned char buf[4]) {
	return buf[0] | buf[1] << 8 | buf[2] << 16 | buf[3] << 24;
}

float BinaryInputFileReader::convertFloatLittleEndian(unsigned char buf[4]) {
	int32_t ibuf = convertIntLittleEndian(buf);

	return *reinterpret_cast<float*>(&ibuf);
}

bool BinaryInputFileReader::isNegativeInfinity(unsigned char buf[4]) {
	return (buf[0] == 0x00) && (buf[1] == 0x00) && (buf[2] == 0x80) && (buf[3] == 0xFF);
}

int32_t BinaryInputFileReader::readIntLittleEndian(fstream& file) {
	unsigned char buf[4];
	file.read((char*)buf, 4);
	return convertIntLittleEndian(buf);
}

float BinaryInputFileReader::readFloatLittleEndian(fstream& file) {
	unsigned char buf[4];
	file.read((char*)buf, 4);
	return convertFloatLittleEndian(buf);
}
