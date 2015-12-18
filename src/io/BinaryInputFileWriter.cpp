#include "io/BinaryInputFileWriter.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

using namespace std;

void BinaryInputFileWriter::write(const string& writeStrategy,
		shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor,
		bool compress) {

	fstream file;
	file.open(writeStrategy, ios::out | ios::binary);

	if (file.good()) {
		cout << "Output file opened." << endl;

		int fileType = compress ? 1 : 2;
		file.write(reinterpret_cast<char*>(&fileType), 4);

		int values = 16;
		file.write(reinterpret_cast<char*>(&values), 4);

		int stageBits = tuplesDescriptor->stageBits;
		file.write(reinterpret_cast<char*>(&stageBits), 4);

		if (compress) {
			for (int s=0; s<(1<<stageBits); s++) {
				int T = tuplesDescriptor->tuples[s].size();
				file.write(reinterpret_cast<char*>(&T), 4);

				for (int i=0; i<T; i++) {
					ExpandedTuple* t = &tuplesDescriptor->tuples[s][i];
					int n = t->pts[0].size();
					int m = t->pts.size();
					file.write(reinterpret_cast<char*>(&n), 4);
					file.write(reinterpret_cast<char*>(&m), 4);
					for (int j=0; j<m; j++) {
						for (int k=0; k<n; k++) {
							int pt = t->pts[j][k];
							file.write(reinterpret_cast<char*>(&pt), 4);
						}
					}
					for (int j=0; j<(1 << 4*(n-1)); j++) {
						// file.write(reinterpret_cast<char*>(&t->lut[j]), 4);
						bool zeros = true;
						for (int k=0; k<16; k++) {
							if (t->lut[(j<<4)+k] != 0) {
								zeros = false;
								break;
							}
						}
						if (zeros) {
							unsigned char buf[4] = { 0x00, 0x00, 0x80, 0xFF };
							file.write((char*)buf, 4);
						} else {
							for (int k=0; k<16; k++) {
								file.write(reinterpret_cast<char*>(&t->lut[(j<<4)+k]), 4);
							}
						}
					}
				}
			}
		} else {
			for (int s=0; s<(1<<stageBits); s++) {
				int T = tuplesDescriptor->tuples[s].size();
				file.write(reinterpret_cast<char*>(&T), 4);

				for (int i=0; i<T; i++) {
					ExpandedTuple* t = &tuplesDescriptor->tuples[s][i];
					int n = t->pts[0].size();
					int m = t->pts.size();
					file.write(reinterpret_cast<char*>(&n), 4);
					file.write(reinterpret_cast<char*>(&m), 4);
					for (int j=0; j<m; j++) {
						for (int k=0; k<n; k++) {
							int pt = t->pts[j][k];
							file.write(reinterpret_cast<char*>(&pt), 4);
						}
					}
					for (int j=0; j<(1 << 4*n); j++) {
						file.write(reinterpret_cast<char*>(&t->lut[j]), 4);
					}
				}
			}
		}
	} else {
		cerr << "Error while opening an output file." << endl;
	}

	file.close();
}
