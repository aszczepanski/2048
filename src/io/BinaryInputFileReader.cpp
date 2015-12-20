#include "io/BinaryInputFileReader.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

#define NDEBUG
#include <cassert>

using namespace std;

shared_ptr<TuplesDescriptor>
BinaryInputFileReader::read(const string& strategy, bool unzip) {
	shared_ptr<TuplesDescriptor> tuplesDescriptor;

	auto startTimePoint = chrono::system_clock::now();

	fstream file;
	file.open(strategy, ios::in | ios::binary);

	if (file.good()) {
		cout << "Binary input file was opened and is being read." << endl;

		tuplesDescriptor = readOpenedBinaryFile(file, unzip);

		if (tuplesDescriptor == nullptr) {
			cerr << "Error while reading an input file." << endl;
		} else {
			auto endTimePoint = chrono::system_clock::now();
			auto duration = chrono::duration_cast< chrono::duration<float> >
				(endTimePoint - startTimePoint);

			cout << "Strategy file was read in "
				<< fixed << setprecision(2) << duration.count()
				<< " seconds." << endl;
		}
	} else {
		cerr << "Error while opening an input file." << endl;
	}

	file.close();

	return tuplesDescriptor;
}

shared_ptr<TuplesDescriptor>
BinaryInputFileReader::readOpenedBinaryFile(fstream& file, bool unzip) {
	shared_ptr<TuplesDescriptor> tuplesDescriptor;

	unsigned char buf[4];

	int fileType;
	file.read((char*)buf, 4);
	fileType = convertIntLittleEndian(buf);

	int values;
	file.read((char*)buf, 4);
	values = convertIntLittleEndian(buf);
	assert(values == 16);
	if (values != 16) return nullptr;

	try {
		if (fileType == 2) {
			tuplesDescriptor = readExpandedFile(file);
		} else if (unzip) {
			tuplesDescriptor = readAndExpandCompressedFile(file);
		} else {
			tuplesDescriptor = readCompressedFile(file);
		}
	} catch (...) {
		return nullptr;
	}

	return tuplesDescriptor;
}

shared_ptr<ExpandedTuplesDescriptor> BinaryInputFileReader::readExpandedFile(fstream& file) {
	shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor = make_shared<ExpandedTuplesDescriptor>();

	tuplesDescriptor->stageBits = readIntLittleEndian(file);
	tuplesDescriptor->stageBitsOffset = 16-tuplesDescriptor->stageBits;
	tuplesDescriptor->tuples.resize(1<<tuplesDescriptor->stageBits);

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		int T = readIntLittleEndian(file);
		tuplesDescriptor->tuples.at(s).resize(T);
		for (int i=0; i<T; i++) {
			ExpandedTuple* t = &tuplesDescriptor->tuples.at(s).at(i);
			int n = readIntLittleEndian(file);
			int m = readIntLittleEndian(file);
			t->pts.resize(m);

			for (int j=0; j<m; j++) {
				t->pts.at(j).resize(n);
				for (int k=0; k<n; k++) {
					t->pts.at(j).at(k) = readIntLittleEndian(file);
				}
			}

			t->lut.resize(1 << 4*n);
			for (int j=0; j<(1 << 4*n); j++) {
				t->lut.at(j) = readFloatLittleEndian(file);
			}
		}
	}

	assert(file.good());
	if (!file.good()) return nullptr;

	return tuplesDescriptor;
}

shared_ptr<ExpandedTuplesDescriptor> BinaryInputFileReader::readAndExpandCompressedFile(fstream& file) {
	shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor = make_shared<ExpandedTuplesDescriptor>();

	unsigned char buf[4];

	tuplesDescriptor->stageBits = readIntLittleEndian(file);
	tuplesDescriptor->stageBitsOffset = 16-tuplesDescriptor->stageBits;
	tuplesDescriptor->tuples.resize(1<<tuplesDescriptor->stageBits);

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		int T = readIntLittleEndian(file);
		tuplesDescriptor->tuples.at(s).resize(T);
		for (int i=0; i<T; i++) {
			ExpandedTuple* t = &tuplesDescriptor->tuples.at(s).at(i);
			int n = readIntLittleEndian(file);
			int m = readIntLittleEndian(file);
			t->pts.resize(m);

			for (int j=0; j<m; j++) {
				t->pts.at(j).resize(n);
				for (int k=0; k<n; k++) {
					t->pts.at(j).at(k) = readIntLittleEndian(file);
				}
			}

			t->lut.resize(1 << 4*n);
			for (int j=0; j<(1 << 4*(n-1)); j++) {
				file.read((char*)buf, 4);
				if (isNegativeInfinity(buf)) {
					for (int k=0; k<16; k++) {
						t->lut.at((j<<4)+k) = 0;
					}
				} else {
					float firstVal = convertFloatLittleEndian(buf);
					t->lut.at(j<<4) = firstVal;
					for (int k=1; k<16; k++) {
						t->lut.at((j<<4)+k) = readFloatLittleEndian(file);
					}
				}
			}
		}
	}

	assert(file.good());
	if (!file.good()) return nullptr;

	return tuplesDescriptor;
}

shared_ptr<CompressedTuplesDescriptor> BinaryInputFileReader::readCompressedFile(fstream& file) {
	shared_ptr<CompressedTuplesDescriptor> tuplesDescriptor = make_shared<CompressedTuplesDescriptor>();

	shared_ptr< array<TupleValueType, 16> > nullArray = make_shared< array<TupleValueType, 16> >();
	nullArray->fill(0.0);

	unsigned char buf[4];

	tuplesDescriptor->stageBits = readIntLittleEndian(file);
	tuplesDescriptor->stageBitsOffset = 16-tuplesDescriptor->stageBits;
	tuplesDescriptor->tuples.resize(1<<tuplesDescriptor->stageBits);

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		int T = readIntLittleEndian(file);
		tuplesDescriptor->tuples.at(s).resize(T);
		for (int i=0; i<T; i++) {
			CompressedTuple* t = &tuplesDescriptor->tuples.at(s).at(i);
			int n = readIntLittleEndian(file);
			int m = readIntLittleEndian(file);
			t->pts.resize(m);

			for (int j=0; j<m; j++) {
				t->pts.at(j).resize(n);
				for (int k=0; k<n; k++) {
					t->pts.at(j).at(k) = readIntLittleEndian(file);
				}
			}

			t->lutPtrs.resize(1 << 4*(n-1));
			for (int j=0; j<(1 << 4*(n-1)); j++) {
				file.read((char*)buf, 4);
				if (isNegativeInfinity(buf)) {
					t->lutPtrs.at(j) = nullArray;
				} else {
					float firstVal = convertFloatLittleEndian(buf);
					t->lutPtrs.at(j) = make_shared<array<TupleValueType, 16> >();
					t->lutPtrs.at(j)->at(0) = firstVal;
					for (int k=1; k<16; k++) {
						t->lutPtrs.at(j)->at(k) = readFloatLittleEndian(file);
					}
				}
			}
		}
	}

	assert(file.good());
	if (!file.good()) return nullptr;

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
