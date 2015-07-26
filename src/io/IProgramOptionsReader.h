#ifndef SRC_IO_IPROGRAM_OPTIONS_READER_H_
#define SRC_IO_IPROGRAM_OPTIONS_READER_H_

#include <memory>

#include "common/ProgramOptions.h"

class IProgramOptionsReader {
public:
	virtual std::shared_ptr<ProgramOptions> read(int argc, char* argv[]) = 0;
};

#endif  // SRC_IO_IPROGRAM_OPTIONS_READER_H_
