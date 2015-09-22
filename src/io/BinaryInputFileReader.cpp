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

		cout << "file type = " << fileType << endl;

		int values;
		file.read((char*)buf, 4);
		values = convertIntLittleEndian(buf);
		// cout << values << endl;
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

	unsigned char buf[4];

	file.read((char*)buf, 4);
	// printf("0x%X 0x%X 0x%X 0x%X\n", buf[0], buf[1], buf[2], buf[3]);
	tuplesDescriptor->stageBits = convertIntLittleEndian(buf);
	tuplesDescriptor->tuples.resize(1<<tuplesDescriptor->stageBits);
	tuplesDescriptor->T.resize(1<<tuplesDescriptor->stageBits);
	// cout << "stage bits = " << tuplesDescriptor->stageBits << endl;

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		file.read((char*)buf, 4);
		tuplesDescriptor->T[s] = convertIntLittleEndian(buf);
		tuplesDescriptor->tuples[s].resize(tuplesDescriptor->T[s]);
		// cout << "reading stage: " << s << endl;
		// cout << "T = " << tuplesDescriptor->T[s] << endl;
		for (int i=0; i<tuplesDescriptor->T[s]; i++) {
			ExpandedTuple* t = &tuplesDescriptor->tuples[s][i];
			file.read((char*)buf, 4);
			t->n = convertIntLittleEndian(buf);
			file.read((char*)buf, 4);
			t->m = convertIntLittleEndian(buf);
			t->pts.resize(t->m);

			// cout << "n = " << t->n << ", m = " << t->m << endl;

			for (int j=0; j<t->m; j++) {
				t->pts[j].resize(t->n);
				for (int k=0; k<t->n; k++) {
					file.read((char*)buf, 4);
					// t->pts[j][k] = positionMapping[convertIntLittleEndian(buf)];
					t->pts[j][k] = convertIntLittleEndian(buf);
					// cout << t->pts[j][k] << " ";
				}
				// cout << endl;
			}

			t->lut.resize(1 << 4*t->n);
			for (int j=0; j<(1 << 4*t->n); j++) {
				file.read((char*)buf, 4);
				t->lut[j] = convertFloatLittleEndian(buf);
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

	file.read((char*)buf, 4);
	// printf("0x%X 0x%X 0x%X 0x%X\n", buf[0], buf[1], buf[2], buf[3]);
	tuplesDescriptor->stageBits = convertIntLittleEndian(buf);
	tuplesDescriptor->tuples.resize(1<<tuplesDescriptor->stageBits);
	tuplesDescriptor->T.resize(1<<tuplesDescriptor->stageBits);
	// cout << "stage bits = " << tuplesDescriptor->stageBits << endl;

	for (int s=0; s<(1<<tuplesDescriptor->stageBits); s++) {
		file.read((char*)buf, 4);
		tuplesDescriptor->T[s] = convertIntLittleEndian(buf);
		tuplesDescriptor->tuples[s].resize(tuplesDescriptor->T[s]);
		// cout << "T = " << tuplesDescriptor->T[s] << endl;
		for (int i=0; i<tuplesDescriptor->T[s]; i++) {
			CompressedTuple* t = &tuplesDescriptor->tuples[s][i];
			file.read((char*)buf, 4);
			t->n = convertIntLittleEndian(buf);
			file.read((char*)buf, 4);
			t->m = convertIntLittleEndian(buf);
			t->pts.resize(t->m);

			// cout << "n = " << t->n << ", m = " << t->m << endl;

			for (int j=0; j<t->m; j++) {
				t->pts[j].resize(t->n);
				for (int k=0; k<t->n; k++) {
					file.read((char*)buf, 4);
					// t->pts[j][k] = positionMapping[convertIntLittleEndian(buf)];
					t->pts[j][k] = convertIntLittleEndian(buf);
					// cout << t->pts[j][k] << " ";
				}
				// cout << endl;
			}

			t->lutPtrs.resize(1 << 4*(t->n-1));
			for (int j=0; j<(1 << 4*(t->n-1)); j++) {
				file.read((char*)buf, 4);
				// printf("0x%X 0x%X 0x%X 0x%X\n", buf[0], buf[1], buf[2], buf[3]);
				if (isNegativeInfinity(buf)) {
					// cout << "FLT_MIN" << endl;
					// t->lutPtrs[j] = make_shared<array<TupleValueType, 16> >();
					// t->lutPtrs[j]->fill(0);
					t->lutPtrs[j] = nullArray;
				} else {
					float firstVal = convertFloatLittleEndian(buf);
					// cout << firstVal << endl;
					t->lutPtrs[j] = make_shared<array<TupleValueType, 16> >();
					t->lutPtrs[j]->at(0) = firstVal;
					for (int k=1; k<16; k++) {
						file.read((char*)buf, 4);
						t->lutPtrs[j]->at(k) = convertFloatLittleEndian(buf);
						// cout << t->lutPtrs[j]->at(k) << endl;
						// printf("0x%X 0x%X 0x%X 0x%X\n", buf[0], buf[1], buf[2], buf[3]);
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
