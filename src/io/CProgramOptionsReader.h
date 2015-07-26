#ifndef SRC_IO_CPROGRAM_OPTIONS_READER_H_
#define SRC_IO_CPROGRAM_OPTIONS_READER_H_

#include "io/IProgramOptionsReader.h"

#include <memory>

#include "common/ProgramOptions.h"

class CProgramOptionsReader : public IProgramOptionsReader {
public:
	virtual std::shared_ptr<ProgramOptions> read(int argc, char* argv[]);
};

#endif  // SRC_IO_CPROGRAM_OPTIONS_READER_H_
