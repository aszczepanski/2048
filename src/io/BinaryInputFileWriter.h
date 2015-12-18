#ifndef SRC_IO_BINARY_INPUT_FILE_WRITER_H_
#define SRC_IO_BINARY_INPUT_FILE_WRITER_H_

#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

class BinaryInputFileWriter {
public:
	void write(const std::string& writeStrategy,
		std::shared_ptr<ExpandedTuplesDescriptor> tuplesDescriptor,
		bool compress = true);

};


#endif  // SRC_IO_BINARY_INPUT_FILE_WRITER_H_
