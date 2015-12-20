#ifndef SRC_IO_BINARY_INPUT_FILE_READER_H_
#define SRC_IO_BINARY_INPUT_FILE_READER_H_

#include "io/IInputFileReader.h"

#include <cstdint>
#include <fstream>
#include <map>
#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

class BinaryInputFileReader : public IInputFileReader {
public:
	virtual std::shared_ptr<TuplesDescriptor> read(const std::string& strategy, bool unzip);

private:
	std::shared_ptr<TuplesDescriptor> readOpenedBinaryFile(std::fstream&, bool unzip);

	std::shared_ptr<ExpandedTuplesDescriptor> readExpandedFile(std::fstream&);
	std::shared_ptr<CompressedTuplesDescriptor> readCompressedFile(std::fstream&);
	std::shared_ptr<ExpandedTuplesDescriptor> readAndExpandCompressedFile(std::fstream&);

	int32_t convertIntLittleEndian(unsigned char buf[4]);
	float convertFloatLittleEndian(unsigned char buf[4]);

	int32_t readIntLittleEndian(std::fstream&);
	float readFloatLittleEndian(std::fstream&);

	bool isNegativeInfinity(unsigned char buf[4]);
};

#endif  // SRC_IO_BINARY_INPUT_FILE_READER_H_
