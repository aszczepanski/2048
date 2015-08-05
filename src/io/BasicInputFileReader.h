#ifndef SRC_IO_BASIC_INPUT_FILE_READER_H_
#define SRC_IO_BASIC_INPUT_FILE_READER_H_

#include "io/IInputFileReader.h"

#include <memory>
#include <string>

#include "common/TuplesDescriptor.h"

class BasicInputFileReader : public IInputFileReader {
public:
	virtual std::shared_ptr<TuplesDescriptor> read(const std::string& strategy);

};

#endif  // SRC_IO_BASIC_INPUT_FILE_READER_H_
