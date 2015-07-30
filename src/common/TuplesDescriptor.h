#ifndef SRC_COMMON_TUPLES_DESCRIPTOR_H_
#define SRC_COMMON_TUPLES_DESCRIPTOR_H_

#include <memory>
#include <vector>

struct Tuple {
	int n;
	int m;

	std::vector< std::vector<int> > pts;  // of size m*n
	std::vector<float> lut;  // of size 16^n
};

struct TuplesDescriptor {
	int T;
	std::vector<Tuple> tuples;
};

#endif  // SRC_COMMON_TUPLES_DESCRIPTOR_H_
