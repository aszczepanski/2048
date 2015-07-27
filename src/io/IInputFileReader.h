#ifndef SRC_IO_IINPUT_FILE_READER_H_
#define SRC_IO_IINPUT_FILE_READER_H_

#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

class IInputFileReader {
public:
	virtual std::shared_ptr<TuplesDescriptor> read(const std::string& filename) = 0;
};

#endif  // SRC_IO_IINPUT_FILE_READER_H_
